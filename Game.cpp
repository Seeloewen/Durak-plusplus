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
		trump = cardStack.back()->type;
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
			if(isServer) cardStack.push_back(card);
		}
	}
}

void Game::drawCard(Player* player)
{
	Card* card = cardStack.front();
	cardStack.erase(cardStack.begin());

	player->addCard(card, false);
}

void Game::tick()
{
	if (currentAttack == nullptr)
	{
		currentAttack = new Attack(getPlayer(0), getPlayer(- 1), getPlayer(+ 1), false);
	}

	//Start a new attack if last one has finished
	if (currentAttack->isFinished)
	{
		int newDef = 0;

		if (currentAttack->isDefended)
		{
			newDef = currentAttack->defender->id + 1;
		}
		else
		{
			newDef = currentAttack->defender->id + 2;
		}

		currentAttack = new Attack(getPlayer(newDef), getPlayer(newDef - 1), getPlayer(newDef + 1), false);
	}
}

Player* Game::getPlayer(int id)
{
	int index = ((id % 6) + 6) % 6;

	for (Player* p : players)
	{
		if (p->id == id) return p;
	}
}

Card* Game::getCard(CardType type, std::string name)
{
	for (Card* card : cardRegister)
	{
		if (card->type == type && card->name == name) return card;
	}
}