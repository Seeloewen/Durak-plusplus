#include "Attack.h"
#include "Player.h"
#include "NetworkHandler.h"
#include "Game.h"
#include "LogUtil.h"

Attack::Attack(Player* defender, Player* attacker1, Player* attacker2, bool netCall)
{
	logI(std::format("Started attack with defender: {}, attacker 1: {} and attacker 2: {}", std::to_string(defender->id), std::to_string(attacker1->id), std::to_string(attacker2->id)));

	this->defender = defender;
	this->attacker1 = attacker1;
	this->attacker2 = attacker2;

	durak->lblCurrentDefender->setText(QString::fromStdString(std::format("Current Defender: {}", defender->id)));

	if (!netCall) sendPacket(STARTATTACK, std::format("{};{};{}", std::to_string(defender->id), std::to_string(attacker1->id), std::to_string(attacker2->id)));
}

void Attack::addCard(Player* attacker, Card* card, bool netCall)
{
	logI(std::format("Added card {} {} to current attack.", std::to_string(card->type), card->name));

	if (cardPairs.size() < 6)
	{
		cardPairs.push_back(new CardPair(card));
		//attacker->removeCard(card, netCall);
	}

	durak->attackUi->update();

	if (!netCall) sendPacket(ADDTOATTACK, std::format("{};{};{}", attacker->id, card->name, std::to_string(card->type)));
}

void Attack::defend(Card* attack, Card* defense, bool netCall)
{
	logI(std::format("Added card {} {} as defense for {} {} to current attack.", std::to_string(attack->type), attack->name, std::to_string(defense->type), defense->name));

	for (CardPair* pair : cardPairs)
	{
		if (pair->attack == attack)
		{
			pair->defense = defense;
			//defender->removeCard(defense, netCall);
		}
	}

	durak->attackUi->update();

	if (!netCall) sendPacket(DEFEND, std::format("{};{};{};{}", attack->name, std::to_string(attack->type), defense->name, std::to_string(defense->type)));
}

void Attack::leave(Player* player, bool netCall)
{
	//Remove player from attack
	if (player == attacker1)
	{
		attacker1 = nullptr;
	}
	else if (player == attacker2)
	{
		attacker2 = nullptr;
	}
	else if (player == defender)
	{
		defender = nullptr;
	}

	//Check if everyone left attack
	if (defender == nullptr && attacker1 == nullptr && attacker2 == nullptr) isFinished == true;

	//Check if attack was defended successfully
	bool def = true;
	for (CardPair* pair : cardPairs)
	{
		if (pair->defense == nullptr) def = false;
	}
	isDefended = def;

	if (!netCall) sendPacket(LEAVEATTACK, std::to_string(player->id));
}

Attack::~Attack()
{
	for (CardPair* pair : cardPairs)
	{
		if (isDefended)
		{
			delete pair->attack;
			delete pair->defense;
		}

		delete pair;
	}
}