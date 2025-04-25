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

void Durak::init()
{
	gameTimer = new QTimer(this);
	connect(gameTimer, &QTimer::timeout, this, &Durak::tick);

	this->resize(1280, 720);

	handUi = new HandUi(this);
	attackUi = new AttackUi(this);
	lblPlayerId = new QLabel("Player: {id}", this);
	lblCurrentDefender = new QLabel("Current Defender: {id}", this);
	lblTrump = new QLabel("Trump: {type}", this);
	btnLeaveAttack = new QPushButton("Leave Attack", this);

	connect(btnLeaveAttack, &QPushButton::clicked, this, &Durak::btnLeaveAttack_Clicked);
	btnLeaveAttack->move(width() - 150, 20);
	btnLeaveAttack->resize(120, 50);
	lblCurrentDefender->move(20, 280);
	lblPlayerId->move(20, 250);
	lblCurrentDefender->resize(200, 30);
	lblTrump->move(20, 310);
}

Durak::~Durak()
{
	gameTimer->stop();
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
	game->currentAttack->leave(game->getPlayer(game->playerId), false);
}

//-- HandUi --//

HandUi::HandUi(QWidget* parent) : QWidget(parent)
{
	setGeometry(20, durak->height() - 200, durak->width() - 40, 180);
	setStyleSheet("background-color: rgb(100, 200, 300);");
}

void HandUi::setPlayer(Player* player)
{
	this->player = player;
}

void HandUi::refresh()
{
	//Clear previous hand
	for (CardUi* cardUi : cards)
	{
		delete cardUi;
	}
	cards.clear();


	int amountCards = player->hand.size();
	int space = 22;
	int prev = space;

	//What even is this :skull:
	if (amountCards > 10) space = 0.0552083333 * amountCards * amountCards * amountCards - 2.76875 * amountCards * amountCards + 41.1791667 * amountCards - 202.125;

	for (Card* card : player->hand)
	{
		int x = prev;
		CardUi* cardUi = new CardUi(x, 20, this);
		cardUi->setCard(card);
		cards.push_back(cardUi);

		cardUi->onClick = [this, card]() {
			if (game->currentAttack->defender == player)
			{
				game->selectedCard = card;
			}
			else if (game->currentAttack->attacker1 == player
				|| game->currentAttack->attacker2 == player)
			{
				game->currentAttack->addCard(player, card, false);
			}
			};

		prev = prev + space + 100;
	}

	update();
}

//-- AttackUi --//

AttackUi::AttackUi(QWidget* parent) :QWidget(parent)
{
	setGeometry(20, 20, 740, 180);
	setStyleSheet("background-color: rgb(100, 200, 300);");
}

void AttackUi::refresh()
{
	for (CardUi* cardUi : cardUis)
	{
		delete cardUi;
	}
	cardUis.clear();

	int i = 0;
	for (CardPair* pair : game->currentAttack->cardPairs)
	{
		addAttack(pair->attack, i);
		if (pair->defense != nullptr)
		{
			addDefense(pair->defense, i);
		}
		i++;
	}

	update();
}

void AttackUi::addAttack(Card* card, int index)
{
	int x = (index + 1) * 20 + index * 100;
	int y = 20;
	CardUi* cardUi = new CardUi(x, y, this);
	cardUi->setCard(card);
	cardUi->onClick = [card]()
		{
			if (game->selectedCard != nullptr)
			{
				game->currentAttack->defend(card, game->selectedCard, false);
				game->selectedCard = nullptr;
			}
		};

	cardUis.push_back(cardUi);
}

void AttackUi::addDefense(Card* card, int index)
{
	CardUi* cardUi = new CardUi((index + 1) * 20 + index * 100, 40, this);
	cardUi->setCard(card);
	cardUis.push_back(cardUi);
}