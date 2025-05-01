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

void startServer(unsigned int port)
{
	if (client == nullptr && server == nullptr)
	{
		game = new Game();
		server = new Server();

		std::thread serverThread([&]() {
			server->start(port);
			});
		serverThread.detach();
	}
}

void connectAsClient(std::string ip, unsigned int port)
{
	if (client == nullptr && server == nullptr)
	{
		game = new Game();
		client = new Client(0);
		std::thread clientThread([&]() {
			client->connect(ip, port);
			});
		clientThread.detach();
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