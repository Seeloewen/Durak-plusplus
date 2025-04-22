#pragma once

#include <string>
#include <winsock2.h>

inline unsigned int nextId = 1;

class Client
{
public:
	unsigned int id;

	Client(unsigned int id);
	SOCKET clientSocket;
	void connect(std::string ip, unsigned int port);
	void send(std::string message);
	bool sendFull(SOCKET socket, const char* data, int totalLen);
	bool recvFull(SOCKET socket, char* buffer, int totalLen);
};

