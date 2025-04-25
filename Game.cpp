#include <qstring.h>
#include "Player.h"
#include "Card.h"
#include <iostream>
#include <Game.h>
#include "Durak.h"
#include "Attack.h"
#include "LogUtil.h"
#include "NetworkHandler.h"

void Game::init(int playerNum, int playerId)
{
	//Initiliaze the game
	durak->init();
	this->playerAmount = playerNum;

	genCards();

	//Init players
	for (int i = 0; i < playerNum; i++)
	{
		players.push_back(new Player(i));
	}
	durak->handUi->setPlayer(getPlayer(playerId));
	durak->lblPlayerId->setText(QString::fromStdString(std::format("Player: {}", playerId)));

	if (isServer())
	{
		//Shuffle cards and select the trump
		shuffleCards(&cardStack);
		setTrump(cardStack.back()->type);
	}

	if (isClient()) sendPacket(INIT_RESPONSE, "");
	//TODO: Check if "Neumischung" (player has 5 of one card type or more)

	//TODO: Make first defender the one next to guy with lowest trump
}

void Game::initialDraw()
{
	//Give each player 6 cards
	for (Player* player : players)
	{
		for (int i = 0; i < 6; i++)
		{
			drawCard(player);
		}
	}
}

void Game::genCards()
{
	//Init cards
	for (int i = 0; i < 4; ++i)
	{
		CardType type = static_cast<CardType>(i);

		for (int j = 6; j < 15; j++)
		{
			std::string name;

			switch (j)
			{
			case 11:
				name = "Jack";
				break;
			case 12:
				name = "Queen";
				break;
			case 13:
				name = "King";
				break;
			case 14:
				name = "Ace";
				break;
			default:
				name = std::to_string(j);
			}

			//Add card to the register. If server, also add card to the stack
			Card* card = new Card(type, j, name);
			cardRegister.push_back(card);
			if (isServer) cardStack.push_back(card);
		}
	}
}

void Game::setTrump(CardType type)
{
	trump = type;
	durak->lblTrump->setText(QString::fromStdString(std::format("Trump: {}", strFromType(type))));
}

void Game::drawCard(Player* player)
{
	if (cardStack.size() != 0)
	{
		Card* card = cardStack.front();
		cardStack.erase(cardStack.begin());

		player->addCard(card, false);
	}
}

void Game::tick()
{
	//Initial attack
	if (currentAttack == nullptr)
	{
		currentAttack = new Attack(getPlayer(0), getPlayer(-1), getPlayer(+1), false);
	}

	//Start a new attack if last one has finished
	if (currentAttack->isFinished)
	{
		int newDef = 0;

		if (currentAttack->isDefended)
		{
			newDef = currentAttack->defId + 1;
		}
		else
		{
			//If not defended, give all cards to the defender
			for (CardPair* pair : currentAttack->cardPairs)
			{
				getPlayer(currentAttack->defId)->addCard(pair->attack, false);
				if (pair->defense != nullptr) getPlayer(currentAttack->defId)->addCard(pair->defense, false);
			}
			newDef = currentAttack->defId + 2;
		}

		stockUpCards(getPlayer(currentAttack->defId), getPlayer(currentAttack->att1Id), getPlayer(currentAttack->att2Id));

		//Clear the bord and start a new attack
		delete currentAttack;
		currentAttack = new Attack(getPlayer(newDef), getPlayer(newDef - 1), getPlayer(newDef + 1), false);
		durak->attackUi->refresh();
	}
}

void Game::stockUpCards(Player* def, Player* att1, Player* att2)
{
	int i = 0;

	//Defender
	i = def->hand.size();
	if (6 - i > 0)
	{
		for (int j = 0; j < 6 - i; j++)
		{
			drawCard(def);
		}
	}

	//Attacker 1
	i = att1->hand.size();
	if (6 - i > 0)
	{
		for (int j = 0; j < 6 - i; j++)
		{
			drawCard(att1);
		}
	}


	//Attacker 2
	if (att2 != att1)
	{

		i = att2->hand.size();
		if (6 - i > 0)
		{
			for (int j = 0; j < 6 - i; j++)
			{
				drawCard(att2);
			}
		}
	}
}

Player* Game::getPlayer(int id)
{
	int index = ((id % playerAmount) + playerAmount) % playerAmount;
	return players[index];
}

Card* Game::getCard(CardType type, std::string name)
{
	for (Card* card : cardRegister)
	{
		if (card->type == type && card->name == name) return card;
	}
}