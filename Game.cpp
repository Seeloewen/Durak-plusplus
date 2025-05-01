#include "Attack.h"
#include "Card.h"
#include "Durak.h"
#include "Game.h"
#include "LogUtil.h"
#include "NetworkHandler.h"
#include "Player.h"
#include "TextureManager.h"
#include <iostream>
#include <qstring.h>
#include <qtimer.h>

void startGame() //Should only executed by server
{
	game->preInit(server->clients.size() + 1, 0);

	//Start the game for everyone
	for (Client* client : server->clients)
	{
		sendPacketOnly(client->id, INIT, std::format("{};{};{}", std::to_string(server->clients.size() + 1), std::to_string(client->id), std::to_string(game->trump)));
	}
}

void resetGame()
{
	delete game;

	if (isServer())
	{
		sendPacket(RESET);
		startGame();
	}
}

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
	durak->close();
	cleanUpTextures();
}

void Game::preInit(int playerAmount, int playerId)
{
	//Initiliaze the game
	initTextures();
	genCards();
	durak = new Durak();
	this->playerAmount = playerAmount;

	//Init players
	for (int i = 0; i < playerAmount; i++)
	{
		players.push_back(new Player(i));
	}

	setCurrentPlayer(playerId);

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
		sendPacket(ADDTOSTACK, card->id);
	}

	game->initialDraw();

	//Start the game
	sendPacket(START);
	durak->timerStart();
	durak->show();

	//Initial attack
	//TODO: Make first defender the one next to guy with lowest trump
	Attack::createAttack(getPlayer(0), getPlayer(-1), getPlayer(+1), false);

	logI("PostInit completed");
}

void Game::tick()
{
	if (currentAttack->isFinished) //Start a new attack if last one has finished
	{
		//Stock up cards and calculate new defender before finishing
		int newDef = currentAttack->defender->id + (currentAttack->isDefended ? 1 : 2);
		stockUpCards(currentAttack->defender, currentAttack->attacker1, currentAttack->attacker2);

		//Clear the bord and start a new attack
		currentAttack->finish();

		//Check if only one player is remaining (game done), else start a new attack
		if (getFinishedPlayers() >= players.size() - 1)
		{
			endGame();
			return;
		}
		else
		{
			Attack::createAttack(getPlayer(newDef), getPlayer(newDef - 1), getPlayer(newDef + 1), false);
		}
	}
}

void Game::endGame()
{
	if (isServer)
	{
		gameTimer->stop();
		sendPacket(ENDGAME);
	}

	durak->showEndScreen(player->finalPos);
}

int Game::getFinishedPlayers()
{
	//Count the amount of players that are finished
	int playersDone = 0;
	for (Player* player : players)
	{
		if (player->getStatus() == FINISHED)
		{
			playersDone++;
		}
	}

	return playersDone;
}

void Game::setCurrentPlayer(int id)
{
	//Set the player that is playing on this instance
	player = getPlayer(id);
	durak->setPlayer(player);
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
			Card* card = new Card(type, value, Card::nameFromValue(value));
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

Card* getCard(std::string id)
{
	for (Card* card : cardRegister)
	{
		if (card->id == id) return card;
	}
}