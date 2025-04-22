#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "Client.h"
#include <qmetaobject.h>
#include <Game.h>
#include <LogUtil.h>
#include <NetworkHandler.h>

#pragma comment(lib, "ws2_32.lib")

Client::Client(unsigned int id) { this->id = id; }

bool Client::sendFull(SOCKET socket, const char* data, int totalLen) {
    int sent = 0;
    while (sent < totalLen) {
        int result = ::send(socket, data + sent, totalLen - sent, 0);
        if (result <= 0) return false;
        sent += result;
    }
    return true;
}

bool Client::recvFull(SOCKET socket, char* buffer, int totalLen) {
    int received = 0;
    while (received < totalLen) {
        int result = recv(socket, buffer + received, totalLen - received, 0);
        if (result <= 0) return false;
        received += result;
    }
    return true;
}

void Client::connect(std::string ip, unsigned int port) {
    logT(Info, std::format("Connecting to {}:{}...", ip, port));

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        logT(Error, "Error: Could not initialize WinSocket!");
        return;
    }

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        logT(Error, "Error: Could not create socket!");
        WSACleanup();
        return;
    }

    addrinfo hints{}, * res = nullptr;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    std::string portStr = std::to_string(port);

    if (getaddrinfo(ip.c_str(), portStr.c_str(), &hints, &res) != 0) {
        logT(Error, "Error: Could not get address info!");
        closesocket(clientSocket);
        WSACleanup();
        return;
    }

    if (::connect(clientSocket, res->ai_addr, static_cast<int>(res->ai_addrlen)) == SOCKET_ERROR) {
        logT(Error, "Error: Could not connect to the server!");
        closesocket(clientSocket);
        freeaddrinfo(res);
        WSACleanup();
        return;
    }

    freeaddrinfo(res);
    logT(Info, "Connection with the server was successfully established!");

    QObject* d = durak;

    while (true) {
        uint32_t len = 0;
        if (!recvFull(clientSocket, reinterpret_cast<char*>(&len), sizeof(len))) break;
        len = ntohl(len);

        std::string msg(len, '\0');
        if (!recvFull(clientSocket, msg.data(), len)) break;

        QMetaObject::invokeMethod(d, [d, msg]() {
            handlePacket(getPacket(msg));
            }, Qt::QueuedConnection);
    }

    closesocket(clientSocket);
    WSACleanup();
}

void Client::send(std::string message) {
    uint32_t len = htonl(static_cast<uint32_t>(message.size()));
    sendFull(clientSocket, reinterpret_cast<const char*>(&len), sizeof(len));
    sendFull(clientSocket, message.data(), static_cast<int>(message.size()));
}
