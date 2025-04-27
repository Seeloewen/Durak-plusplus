#include "Attack.h"
#include "Player.h"
#include "NetworkHandler.h"
#include "Game.h"
#include "LogUtil.h"
#include "Util.h"

Attack::Attack(Player* defender, Player* attacker1, Player* attacker2)
{
	this->defender = defender;
	this->attacker1 = attacker1;
	this->attacker2 = attacker2;
}

Attack::~Attack()
{
	//Give cards to the players before deleting the pairs
	for (CardPair* pair : cardPairs)
	{
		delete pair;
	}
}

void Attack::createAttack(Player* def, Player* att1, Player* att2, bool netCall)
{
	Attack* attack = new Attack(def, att1, att2);
	game->currentAttack = attack;

	//Update UI
	durak->setDefender(def->id);
	durak->teAttackLog->clear();
	durak->writeAttackLog("Started new attack");
	durak->attackUi->refresh();
	durak->setPlayerStatus(game->player->getStatus()); //TODO: method in "game" to update player status, accounting for future player status widget

	if (!netCall) sendPacket(STARTATTACK, std::format("{};{};{}", std::to_string(def->id), std::to_string(att1->id), std::to_string(att2->id)));
	
	logI(std::format("Started attack with defender: {}, attacker 1: {} and attacker 2: {}", std::to_string(def->id), std::to_string(att1->id), std::to_string(att2->id)));
}

void Attack::finish()
{
	if (!isDefended)
	{
		for (CardPair* pair : cardPairs)
		{
			defender->addCard(pair->attack, false);
			if (pair->defense != nullptr) defender->addCard(pair->defense, false);
		}
	}

	delete this;
}

void Attack::addCard(Player* attacker, Card* card, bool netCall)
{
	if (!karteLiegt(card->value) //Mach die Kadde weg, die liegt nicht!!
		|| vectorContains(quitPlayers, attacker) //Attacker has already left attack
		|| cardPairs.size() == 0 && attacker != attacker1 //Only first attacker can place first card
		|| defender->hand.size() <= getUndefendedAmount() //Not more cards than the defender can defend
		|| cardPairs.size() >= 6) //Not more than 6 cards
		return; 

	//Add new cardpair and remove card from inventory
	cardPairs.push_back(new CardPair(card));
	if (!netCall) attacker->removeCard(card, netCall);
	durak->attackUi->refresh();

	if (!netCall) sendPacket(ADDTOATTACK, std::format("{};{}", attacker->id, card->id));

	logI(std::format("Added card {} to current attack.", card->id));
}

void Attack::defend(Card* attack, Card* defense, bool netCall)
{
	if (vectorContains(quitPlayers, defender)) return; //Defender has already left attack

	if (defense->value > attack->value && attack->type == defense->type //Only allow attack if defense is higher value and same type as attack
		|| defense->type == game->trump && attack->type != game->trump) //Or defense is trump and attack is not
	{
		//Get the correct cardpair and put the defense down, removing the card from the attacker
		for (CardPair* pair : cardPairs)
		{
			if (pair->attack == attack)
			{
				pair->defense = defense;
				if (!netCall) defender->removeCard(defense, netCall);
			}
		}

		durak->attackUi->refresh();
		
		if (!netCall) sendPacket(DEFEND, std::format("{};{}", attack->id, defense->id));
		logI(std::format("Added card {} as defense for {} to current attack.", attack->id, defense->id));
	}
}

void Attack::leave(Player* player, bool netCall)
{
	//Check if attack was defended successfully
	bool def = true;
	for (CardPair* pair : cardPairs)
	{
		if (pair->defense == nullptr) def = false;
	}

	//Remove player from attack
	if (!vectorContains(quitPlayers, player))
	{
		durak->writeAttackLog(std::format("{} left attack", strFromStatus(player->getStatus())));
		quitPlayers.push_back(player);
	}

	//Check if everyone left attack
	if (vectorContains(quitPlayers, attacker1)
		&& vectorContains(quitPlayers, attacker2)
		&& vectorContains(quitPlayers, defender)) isFinished = true;

	//Or: both attackers quit and everything is defended
	if (vectorContains(quitPlayers, attacker1)
		&& vectorContains(quitPlayers, attacker2)
		&& def) isFinished = true;

	if (isFinished) isDefended = def;

	if (!netCall) sendPacket(LEAVEATTACK, std::to_string(player->id));
}

int Attack::getUndefendedAmount()
{
	int undefended = 0;

	//Count all card pairs with undefended attacks
	for (CardPair* pair : cardPairs)
	{
		if (pair->defense == nullptr)
		{
			undefended++;
		}
	}

	return undefended;
}

bool Attack::karteLiegt(int value)
{
	//If the table is empty, every card can be placed
	if (cardPairs.size() == 0) return true;

	//Check all card pairs
	for (CardPair* pair : cardPairs)
	{
		if (pair->attack->value == value //Attack
			|| pair->defense != nullptr && pair->defense->value == value) //Defense
			return true;
	}

	return false;
}