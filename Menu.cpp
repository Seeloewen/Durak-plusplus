#include "Menu.h"
#include "qpushbutton.h"
#include "LogUtil.h"
#include "NetworkHandler.h"
#include "Durak.h"
#include "Game.h"
#include "ui_Menu.h"

Menu::Menu(QWidget* parent) : QDialog(parent)
{
	ui.setupUi(this);
	setAttribute(Qt::WA_QuitOnClose);

	connect(ui.pbConnect, &QPushButton::clicked, this, &Menu::pbConnect_Clicked);
	connect(ui.pbStartServer, &QPushButton::clicked, this, &Menu::pbStartServer_Clicked);
	connect(ui.pbStart, &QPushButton::clicked, this, &Menu::pbStartGame_Clicked);
}

void Menu::pbConnect_Clicked()
{
	connectAsClient(ui.leIp->text().toStdString(), atoi(ui.lePort->text().toStdString().c_str());
}

void Menu::pbStartServer_Clicked()
{
	startServer(5000);
}

void Menu::pbStartGame_Clicked()
{
	if (isServer())
	{
		startGame();
	}
}
