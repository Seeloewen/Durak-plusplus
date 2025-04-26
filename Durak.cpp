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
	lblPlayerStatus = new QLabel(this);
	btnLeaveAttack = new QPushButton("Leave Attack", this);
	teAttackLog = new QTextEdit(this);

	connect(btnLeaveAttack, &QPushButton::clicked, this, &Durak::btnLeaveAttack_Clicked);
	btnLeaveAttack->move(width() - 150, 20);
	btnLeaveAttack->resize(120, 50);
	lblCurrentDefender->move(20, 280);
	lblPlayerId->move(20, 250);
	lblCurrentDefender->resize(200, 30);
	lblTrump->move(20, 310);
	lblPlayerStatus->move(400, handUi->y() - 50);
	lblPlayerStatus->resize(400, 50);
	lblPlayerStatus->setFont(QFont("Arial", 20));
	teAttackLog->resize(300, 150);
	teAttackLog->move(950, handUi->y() - 200);
	teAttackLog->setReadOnly(true);
}

void Durak::setPlayer(Player* player)
{
	handUi->setPlayer(player);
	lblPlayerId->setText(QString::fromStdString(std::format("Player: {}", player->id)));
}

void Durak::setDefender(int id)
{
	lblCurrentDefender->setText(QString::fromStdString(std::format("Current Defender: {}", id)));
}

void Durak::setTrump(CardType type)
{
	lblTrump->setText(QString::fromStdString(std::format("Trump: {}", strFromType(type))));
}

void Durak::setPlayerStatus(PlayerStatus status)
{
	lblPlayerStatus->setText(QString::fromStdString(std::format("You are currently: {}", strFromStatus(status))));
}

void Durak::writeAttackLog(std::string message)
{
	teAttackLog->append(QString::fromStdString(message));
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