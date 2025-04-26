#include "Durak.h"
#include <qlabel.h>
#include <qtimer.h>
#include <Game.h>
#include <Card.h>
#include <qwidget.h>
#include <LogUtil.h>
#include <qpushbutton.h>

//-- Durak --//

Durak::Durak(QWidget* parent) : QMainWindow(parent)
{
	ui.setupUi(this);
}

Durak::~Durak()
{
	if (gameTimer != nullptr)
	{
		gameTimer->stop();
	}
}

void Durak::init()
{
	gameTimer = new QTimer(this);
	connect(gameTimer, &QTimer::timeout, this, &Durak::tick);

	this->resize(1280, 720);

	handUi = new HandUi(this);
	attackUi = new AttackUi(this);
	lblPlayerId = new QLabel(this);
	lblCurrentDefender = new QLabel(this);
	lblTrump = new QLabel(this);
	btnLeaveAttack = new QPushButton("Leave Attack", this);

	connect(btnLeaveAttack, &QPushButton::clicked, this, &Durak::btnLeaveAttack_Clicked);
	btnLeaveAttack->move(width() - 150, 20);
	btnLeaveAttack->resize(120, 50);
	lblCurrentDefender->move(20, 280);
	lblPlayerId->move(20, 250);
	lblCurrentDefender->resize(200, 30);
	lblTrump->move(20, 310);
}

void Durak::setPlayer(Player* player)
{
	durak->handUi->setPlayer(player);
	durak->lblPlayerId->setText(QString::fromStdString(std::format("Player: {}", player->id)));
}

void Durak::setDefender(int id)
{
	lblCurrentDefender->setText(QString::fromStdString(std::format("Current Defender: {}", id)));
}

void Durak::setTrump(CardType type)
{
	durak->lblTrump->setText(QString::fromStdString(std::format("Trump: {}", strFromType(type))));
}

void Durak::tick()
{
	game->tick();
}

void Durak::timerStart()
{
	gameTimer->start(1000 / 60);
}

void Durak::btnLeaveAttack_Clicked()
{
	game->currentAttack->leave(game->player, false);
}