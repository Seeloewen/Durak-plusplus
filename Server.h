#pragma once
#include <winsock2.h>
#include <vector>
#include <mutex>
#include "Client.h"

class Server
{
private:
    SOCKET serverSocket;
    std::mutex mutex_;

    void handleClient(Client* clientSocket);

public:
    std::vector<Client*> clients;
    int initResponses = 0;

    void start(unsigned int port);
    void send(std::string message);
    void sendExcept(std::string message, SOCKET clientSocket);
    void sendOnly(std::string message, SOCKET clientSocket);
};