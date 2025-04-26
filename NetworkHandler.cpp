#include "NetworkHandler.h"
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <Game.h>
#include "LogUtil.h"
#include "Widgets.h"

Packet::Packet(PacketType type, std::vector<std::string> args)
{
	this->type = type;
	this->args = args;
}

Packet* getPacket(std::string message)
{
	std::cout << message << std::endl;
	std::vector<std::string> args;
	PacketType type = INIT;

	//Convert strings into arguments
	std::stringstream ss(message);
	std::string item;

	int i = 0;
	while (std::getline(ss, item, ';'))
	{
		if (i == 0)
		{
			//Get type from arguments
			type = static_cast<PacketType>(atoi(item.c_str()));
		}
		else
		{
			//Put the remaining arguments
			args.push_back(item);
		}

		i++;
	}


	return new Packet(type, args);
}

bool isServer()
{
	return server != nullptr;
}

bool isClient()
{
	return client != nullptr;
}

Client* getClient(int id)
{
	for (Client* client : server->clients)
	{
		if (client->id == id) return client;
	}
}

void sendPacketOnly(int id, PacketType type, std::string message)
{
	//Assumes this is server
	int t = type;
	server->sendOnly(std::to_string(t) + ";" + message, getClient(id)->clientSocket);
}

void sendPacket(PacketType type)
{
	//Send packet without msg
	sendPacket(type, "");
}

void sendPacket(PacketType type, std::string message)
{
	int t = type;

	//Send packet depending on if it's server or client
	if (isServer())
	{
		int t = type;
		server->send(std::to_string(t) + ";" + message);
	}
	else if (isClient())
	{
		int t = type;
		client->send(std::to_string(t) + ";" + message);
	}
}

void handlePacket(Packet* packet)
{
	switch (packet->type)
	{
	case INIT:
		handleInit(packet->args);
		break;
	case INIT_RESPONSE:
		handleInitResponse(packet->args);
		break;
	case ADDCARD:
		handleAddCard(packet->args);
		break;
	case REMOVECARD:
		handleRemoveCard(packet->args);
		break;
	case ADDTOSTACK:
		handleAddToStack(packet->args);
		break;
	case START:
		handleStart(packet->args);
		break;
	case STARTATTACK:
		handleStartAttack(packet->args);
		break;
	case ADDTOATTACK:
		handleAddToAttack(packet->args);
		break;
	case DEFEND:
		handleDefend(packet->args);
		break;
	case LEAVEATTACK:
		handleLeaveAttack(packet->args);
		break;
	default:
		return;
	}
}

void handleInit(std::vector<std::string> args) //Arg Size 2
{
	int playerNum = atoi(args[0].c_str());
	int id = atoi(args[1].c_str());
	CardType trump = static_cast<CardType>(atoi(args[2].c_str()));

	logI(std::format("Received id {} from network", id));

	client->id = id;
	game->preInit(playerNum, id);
	game->setPlayer(id);
	game->setTrump(trump);
}

void handleInitResponse(std::vector<std::string> args) //Arg Size 0
{
	initResponses++;

	if(isServer())
	{
		//Start game when all clients have been initialized
		if (initResponses == server->clients.size())
		{
			game->postInit();
		}
	}
}

void handleAddCard(std::vector<std::string> args)
{
	//Arg Size 3
	Player* player = game->getPlayer(atoi(args[0].c_str()));
	CardType type = static_cast<CardType>(atoi(args[1].c_str()));
	std::string name = args[2];

	player->addCard(game->getCard(type, name), true);
}

void handleRemoveCard(std::vector<std::string> args)
{
	//Arg Size 3
	Player* player = game->getPlayer(atoi(args[0].c_str()));
	CardType type = static_cast<CardType>(atoi(args[1].c_str()));
	std::string name = args[2];

	player->removeCard(game->getCard(type, name), true);
}

void handleAddToStack(std::vector<std::string> args)
{
	//Arg Size 3
	std::string name = args[0];
	CardType type = static_cast<CardType>(atoi(args[1].c_str()));
	int value = atoi(args[2].c_str());

	game->cardStack.push_back(new Card(type, value, name));
}

void handleStart(std::vector<std::string> args)
{
	//Arg Size 0
	durak->show();
}

void handleStartAttack(std::vector<std::string> args)
{
	//Arg Size 3
	int defId = atoi(args[0].c_str());
	int attId1 = atoi(args[1].c_str());
	int attId2 = atoi(args[2].c_str());

	game->currentAttack = new Attack(game->getPlayer(defId), game->getPlayer(attId1), game->getPlayer(attId2), true);
	durak->attackUi->refresh();
	game->updatePlayerStatus();
}

void handleAddToAttack(std::vector<std::string> args)
{
	//Arg Size 2
	int attId = atoi(args[0].c_str());
	std::string name = args[1];
	CardType type = static_cast<CardType>(atoi(args[2].c_str()));

	game->currentAttack->addCard(game->getPlayer(attId), game->getCard(type, name), true);
}

void handleDefend(std::vector<std::string> args)
{
	//Arg Size 4
	std::string nameAtt = args[0];
	CardType typeAtt = static_cast<CardType>(atoi(args[1].c_str()));
	std::string nameDef = args[2];
	CardType typeDef = static_cast<CardType>(atoi(args[3].c_str()));

	game->currentAttack->defend(game->getCard(typeAtt, nameAtt), game->getCard(typeDef, nameDef), true);
}

void handleLeaveAttack(std::vector<std::string> args)
{
	//Arg Size 1
	int id = atoi(args[0].c_str());

	game->currentAttack->leave(game->getPlayer(id), true);
}