#include "Menu.h"
#include "qpushbutton.h"
#include "LogUtil.h"
#include "NetworkHandler.h"
#include "Durak.h"
#include "Game.h"
#include "ui_Menu.h"

Menu::Menu(QWidget* parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	connect(ui.pbConnect, &QPushButton::clicked, this, &Menu::pbConnect_Clicked);
	connect(ui.pbStartServer, &QPushButton::clicked, this, &Menu::pbStartServer_Clicked);
	connect(ui.pbStart, &QPushButton::clicked, this, &Menu::pbStartGame_Clicked);
}

int Menu::exec()
{
	QDialog::exec();
	return result;
}

void Menu::pbConnect_Clicked()
{
	game = new Game();
	client = new Client(0);
	std::thread clientThread([&]() {
		client->connect((ui.leIp->text().toStdString()), atoi(ui.lePort->text().toStdString().c_str()));
		});
	clientThread.detach();
}

void Menu::pbStartServer_Clicked()
{
	game = new Game();
	server = new Server();
	
	std::thread serverThread([&]() {
		server->start(5000);
		});
	serverThread.detach();

	result = 2;
}

void Menu::pbStartGame_Clicked()
{
	game->init(server->clients.size() + 1, 0);

	//Start the game for everyone
	for (Client* client : server->clients)
	{
		sendPacketOnly(client->id, INIT, std::format("{};{}", std::to_string(server->clients.size() + 1), std::to_string(client->id)));
	}
}
