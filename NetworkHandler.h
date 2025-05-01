#pragma once

#include <string>
#include "Server.h"
#include "Client.h"

inline Server* server = nullptr;
inline Client* client = nullptr;

enum PacketType
{
	INIT = 0,
	INIT_RESPONSE = 1,
	ADDCARD = 2,
	REMOVECARD = 3,
	ADDTOSTACK = 4,
	START = 5,
	STARTATTACK = 6,
	ADDTOATTACK = 7,
	DEFEND = 8,
	LEAVEATTACK = 9,
	ENDGAME = 10,
	RESET = 11
};

struct Packet
{
	Packet(PacketType type, std::vector<std::string> args);
	PacketType type;
	std::vector<std::string> args;
};

Packet* getPacket(std::string message);
Client* getClient(int id);
bool isServer();
bool isClient();
void sendPacket(PacketType type);
void sendPacket(PacketType type, std::string message);
void sendPacketOnly(int id, PacketType type, std::string message);
void handlePacket(Packet* packet);
void startServer(unsigned int port);
void connectAsClient(std::string ip, unsigned int port);

void handleInit(std::vector<std::string> args);
void handleInitResponse(std::vector<std::string> args);
void handleAddCard(std::vector<std::string> args);
void handleRemoveCard(std::vector<std::string> args);
void handleAddToStack(std::vector<std::string> args);
void handleStart(std::vector<std::string> args);
void handleStartAttack(std::vector<std::string> args);
void handleAddToAttack(std::vector<std::string> args);
void handleDefend(std::vector<std::string> args);
void handleLeaveAttack(std::vector<std::string> args);
void handleEndGame(std::vector<std::string> args);
void handleReset(std::vector<std::string> args);
