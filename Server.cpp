#include <iostream>
#include <winsock2.h>
#include <thread>
#include <vector>
#include <mutex>
#include "Server.h"
#include "Client.h"
#include "NetworkHandler.h"
#include <qmetaobject.h>
#include <Game.h>
#include <LogUtil.h>

#pragma comment(lib, "ws2_32.lib")

bool sendFull(SOCKET socket, const char* data, int totalLen) {
    int sent = 0;
    while (sent < totalLen) {
        int result = send(socket, data + sent, totalLen - sent, 0);
        if (result <= 0) return false;
        sent += result;
    }
    return true;
}

bool recvFull(SOCKET socket, char* buffer, int totalLen) {
    int received = 0;
    while (received < totalLen) {
        int result = recv(socket, buffer + received, totalLen - received, 0);
        if (result <= 0) return false;
        received += result;
    }
    return true;
}

void Server::start(unsigned int port) {
    logT(Info, std::format("Starting server on port {}...", port));

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        logT(Error, "Error: Could not initialize WinSocket!");
        return;
    }

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        logT(Error, "Error: Could not create server socket!");
        WSACleanup();
        return;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        logT(Error, "Error: Could not bind socket to address!");
        closesocket(serverSocket);
        WSACleanup();
        return;
    }

    if (listen(serverSocket, 5) == SOCKET_ERROR) {
        logT(Error, "Error: Could not listen to clients!");
        closesocket(serverSocket);
        WSACleanup();
        return;
    }

    logT(Info, "Server was started successfully!");
    std::vector<std::thread> clientThreads;

    while (true) {
        Client* client = new Client(nextId++);
        client->clientSocket = accept(serverSocket, nullptr, nullptr);

        if (client->clientSocket != INVALID_SOCKET) {
            logT(Info, std::format("Connected to client #{}!", client->id));
            {
                std::lock_guard<std::mutex> lock(mutex_);
                clients.push_back(client);
            }

            clientThreads.emplace_back(&Server::handleClient, this, client);
        }
    }

    for (auto& t : clientThreads) t.join();
}

void Server::handleClient(Client* client) {
    SOCKET socket = client->clientSocket;
    QObject* d = durak;

    while (true) {
        uint32_t len = 0;
        if (!recvFull(socket, reinterpret_cast<char*>(&len), sizeof(len))) break;
        len = ntohl(len);

        std::string msg(len, '\0');
        if (!recvFull(socket, msg.data(), len)) break;

        QMetaObject::invokeMethod(d, [d, msg]() {
            handlePacket(getPacket(msg));
            }, Qt::QueuedConnection);

        sendExcept(msg, socket);
    }

    std::lock_guard<std::mutex> lock(mutex_);
    clients.erase(std::remove(clients.begin(), clients.end(), client), clients.end());
    closesocket(socket);
    logT(Info, std::format("Client #{} has disconnected", client->id));
}

void Server::send(std::string message) {
    std::lock_guard<std::mutex> lock(mutex_);
    uint32_t len = htonl(static_cast<uint32_t>(message.size()));
    for (Client* client : clients) {
        sendFull(client->clientSocket, reinterpret_cast<const char*>(&len), sizeof(len));
        sendFull(client->clientSocket, message.data(), static_cast<int>(message.size()));
    }
}

void Server::sendExcept(std::string message, SOCKET exceptSocket) {
    std::lock_guard<std::mutex> lock(mutex_);
    uint32_t len = htonl(static_cast<uint32_t>(message.size()));
    for (Client* client : clients) {
        if (client->clientSocket != exceptSocket) {
            sendFull(client->clientSocket, reinterpret_cast<const char*>(&len), sizeof(len));
            sendFull(client->clientSocket, message.data(), static_cast<int>(message.size()));
        }
    }
}

void Server::sendOnly(std::string message, SOCKET targetSocket) {
    uint32_t len = htonl(static_cast<uint32_t>(message.size()));
    sendFull(targetSocket, reinterpret_cast<const char*>(&len), sizeof(len));
    sendFull(targetSocket, message.data(), static_cast<int>(message.size()));
}
