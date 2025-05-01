#include "NetworkHandler.h"
#include "Client.h"
#include "Game.h"
#include "LogUtil.h"

void handleInit(std::vector<std::string> args) //Arg Size 2
{
	int playerNum = atoi(args[0].c_str());
	int id = atoi(args[1].c_str());
	CardType trump = static_cast<CardType>(atoi(args[2].c_str()));

	logI(std::format("Received id {} from network", id));

	client->id = id;
	game->preInit(playerNum, id);
	game->setCurrentPlayer(id);
	game->setTrump(trump);
}

void handleInitResponse(std::vector<std::string> args) //Arg Size 0
{
	server->initResponses++;

	if (isServer())
	{
		//Start game when all clients have been initialized
		if (server->initResponses == server->clients.size())
		{
			game->postInit();
		}
	}
}

void handleAddCard(std::vector<std::string> args) //Arg Size 2
{
	Player* player = game->getPlayer(atoi(args[0].c_str()));
	std::string cardId = args[1];

	player->addCard(getCard(cardId), true);
}

void handleRemoveCard(std::vector<std::string> args) //Arg Size 2
{
	Player* player = game->getPlayer(atoi(args[0].c_str()));
	std::string cardId = args[1];

	player->removeCard(getCard(cardId), true);
}

void handleAddToStack(std::vector<std::string> args) //Arg Size 1
{
	std::string id = args[0];

	game->cardStack.push_back(getCard(id));
}

void handleStart(std::vector<std::string> args)	//Arg Size 0
{
	durak->show();
}

void handleStartAttack(std::vector<std::string> args) //Arg Size 3
{
	int defId = atoi(args[0].c_str());
	int attId1 = atoi(args[1].c_str());
	int attId2 = atoi(args[2].c_str());

	Attack::createAttack(game->getPlayer(defId), game->getPlayer(attId1), game->getPlayer(attId2), true);
}

void handleAddToAttack(std::vector<std::string> args) //Arg Size 2
{
	int attId = atoi(args[0].c_str());
	std::string cardId = args[1];

	game->currentAttack->addCard(game->getPlayer(attId), getCard(cardId), true);
}

void handleDefend(std::vector<std::string> args) //Arg Size 2
{
	std::string attId = args[0];
	std::string defId = args[1];

	game->currentAttack->defend(getCard(attId), getCard(defId), true);
}

void handleLeaveAttack(std::vector<std::string> args) //Arg Size 1
{
	int id = atoi(args[0].c_str());
	game->currentAttack->leave(game->getPlayer(id), true);
}

void handleEndGame(std::vector<std::string> args) //Arg Size 0
{
	game->endGame();
}

void handleReset(std::vector<std::string> args) //Arg Size 0
{
	resetGame();
}