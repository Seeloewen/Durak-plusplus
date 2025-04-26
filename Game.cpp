#include <qstring.h>
#include "Player.h"
#include "Card.h"
#include <iostream>
#include "Game.h"
#include "Durak.h"
#include "Attack.h"
#include "LogUtil.h"
#include "NetworkHandler.h"
#include "TextureManager.h"

Game::~Game()
{
	for (Player* p : players)
	{
		delete p;
	}

	for (Card* card : cardRegister)
	{
		delete card;
	}

	delete currentAttack;

	cleanUpTextures();
}

void Game::preInit(int playerAmount, int playerId)
{
	//Initiliaze the game
	initTextures();
	durak->init();
	genCards();
	this->playerAmount = playerAmount;

	//Init players
	for (int i = 0; i < playerAmount; i++)
	{
		players.push_back(new Player(i));
	}

	setPlayer(playerId);

	if (isServer())
	{
		//Shuffle cards and select the trump
		shuffleCards(&cardStack);
		setTrump(cardStack.back()->type);
	}

	if (isClient()) sendPacket(INIT_RESPONSE); //If client, let server know init is done

	logI("PreInit completed");
}

void Game::postInit() //Only run by the server after all clients are initialized
{
	//Synchronize the stack
	for (Card* card : game->cardStack)
	{
		sendPacket(ADDTOSTACK, std::format("{};{};{}", card->name, std::to_string(card->type), std::to_string(card->value)));
	}

	game->initialDraw();

	//Start the game
	sendPacket(START);
	durak->timerStart();
	durak->show();

	logI("PostInit completed");
}

void Game::setPlayer(int id)
{
	//Set the player that is playing on this instance
	player = getPlayer(id);
	durak->setPlayer(player);
}

void Game::updatePlayerStatus()
{
	if (player == currentAttack->attacker1)
	{
		status = ATTACKER1;
		durak->setPlayerStatus(ATTACKER1);
	}
	else if (player == currentAttack->attacker2)
	{
		status = ATTACKER2;
		durak->setPlayerStatus(ATTACKER2);
	}
	else if (player == currentAttack->defender)
	{
		status = DEFENDER;
		durak->setPlayerStatus(DEFENDER);
	}
}

void Game::initialDraw()
{
	bool invalidHand = false;

	//Give each player 6 cards
	for (Player* player : players)
	{
		for (int i = 0; i < 6; i++)
		{
			drawCard(player);
			if (player->invalidHand()) invalidHand = true;
		}
	}

	//Check for "Neumischung"
	if (invalidHand)
	{
		logI("Redrawing cards because a player had an invalid hand");
		for (Player* player : players) player->clearHand();
		initialDraw();
	}
}

void Game::genCards()
{
	//Init cards
	for (int i = 0; i < 4; ++i)
	{
		CardType type = static_cast<CardType>(i);

		for (int value = 6; value < 15; value++)
		{
			//Add card to the register. If server, also add card to the stack
			Card* card = new Card(type, value, nameFromValue(value));
			cardRegister.push_back(card);
			if (isServer) cardStack.push_back(card);
		}
	}
}

void Game::setTrump(CardType type)
{
	trump = type;
	durak->setTrump(type);
}

void Game::drawCard(Player* player)
{
	if (cardStack.size() != 0)
	{
		//Take topmost card from the stack and add it to player's inv
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
		//TODO: Make first defender the one next to guy with lowest trump
		currentAttack = new Attack(getPlayer(0), getPlayer(-1), getPlayer(+1), false);
		game->updatePlayerStatus();
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
			//If not defended, give all cards to the defender
			for (CardPair* pair : currentAttack->cardPairs)
			{
				currentAttack->defender->addCard(pair->attack, false);
				if (pair->defense != nullptr) currentAttack->defender->addCard(pair->defense, false);
			}
			newDef = currentAttack->defender->id + 2;
		}

		stockUpCards(currentAttack->defender, currentAttack->attacker1, currentAttack->attacker2);

		//Clear the bord and start a new attack
		delete currentAttack;
		currentAttack = new Attack(getPlayer(newDef), getPlayer(newDef - 1), getPlayer(newDef + 1), false);
		durak->attackUi->refresh();
		game->updatePlayerStatus();
	}

}

void Game::stockUpCards(Player* def, Player* att1, Player* att2)
{
	int i = 0;

	//Attacker 1
	while (att1->hand.size() < 6)
	{		
		if (cardStack.size() == 0) return;
		drawCard(att1);
	}

	//Attacker 2
	if (att2 != att1)
	{
		while (att2->hand.size() < 6)
		{
			if (cardStack.size() == 0) return;
			drawCard(att2);
		}
	}

	//Defender
	while (def->hand.size() < 6)
	{
		if (cardStack.size() == 0) return;
		drawCard(def);
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