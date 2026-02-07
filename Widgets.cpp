#include "Widgets.h"
#include "Game.h"
#include "TextureManager.h"
#include <qpushbutton.h>
#include <qlabel.h>

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
	if (amountCards > 10) space = (width() - 44 - (amountCards * 100)) / amountCards - 1;

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
	setGeometry(20, 20, 740, 220);
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
	CardUi* cardUi = new CardUi((index + 1) * 20 + index * 100, 60, this);
	cardUi->setCard(card);
	cardUis.push_back(cardUi);
}

//-- CardUi --//

CardUi::CardUi(int x, int y, QWidget* parent) : QWidget(parent)
{
	setGeometry(x, y, 100, 140);

	image = new QLabel(this);
	image->resize(100, 140);

	show();
}

CardUi::~CardUi()
{
	hide();
}

void CardUi::mousePressEvent(QMouseEvent* event)
{
	onClick();
}

void CardUi::setCard(Card* card)
{
	this->card = card;

	QPixmap pixmap = QPixmap::fromImage(*getTexture(card->id));
	QPixmap scaledPixmap = pixmap.scaled(image->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
	image->setPixmap(scaledPixmap);

	update();
}

//-- EndScreen --//

EndScreen::EndScreen(QWidget* parent) : QWidget(parent)
{
	lblHeader = new QLabel("Game Over!", this);
	lblStatus = new QLabel(this);
	pbExit = new QPushButton("Exit", this);
	pbRestart = new QPushButton("Restart", this);

	setGeometry(350, 200, 600, 250);
	setStyleSheet("background-color: rgb(154, 154, 154);");

	lblHeader->move(220, 25);
	lblStatus->move(75, 130);
	pbExit->move(170, 200);
	pbRestart->move(320, 200);
pbRestart->setDisabled(true);

	pbExit->resize(100, 30);
	pbRestart->resize(100, 30);
	lblStatus->resize(500, lblStatus->height());
	lblStatus->setFont(QFont("Arial", 15));
	lblHeader->setFont(QFont("Arial", 20));
	pbExit->setStyleSheet("background-color: white; color: black;");


	connect(pbRestart, &QPushButton::clicked, this, &EndScreen::pbRestart_Click);
	connect(pbExit, &QPushButton::clicked, this, &EndScreen::pbExit_Click);

	hide();
}

void EndScreen::setFinalPos(int finalPos)
{
	if (finalPos == 0)
	{
		lblStatus->setText("You did not get rid of your cards and lost the game!");
	}
	else
	{
		lblStatus->setText(QString::fromStdString(std::format("Congrats! You finished as #{}!", finalPos)));
	}
}

void EndScreen::pbExit_Click()
{
	exit(0);
}

void EndScreen::pbRestart_Click()
{
	resetGame();
	hide();
}