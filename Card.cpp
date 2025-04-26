#include "Card.h"
#include "qlabel.h"
#include "Game.h"
#include "qwidget.h"
#include "qpainter.h"
#include <qstring.h>
#include <algorithm>
#include <random>
#include <qobject.h>

//-- Card --//

Card::Card(CardType type, int value, std::string name)
{
	this->type = type;
	this->value = value;
	this->name = name;
}

std::string strFromType(CardType type)
{
	//Get the string from the type
	switch (type)
	{
	case SPADES:
		return "Spades";
	case DIAMONDS:
		return "Diamonds";
	case HEART:
		return "Heart";
	case CLUBS:
		return "Clubs";
	default:
		return "Unknown";
	}
}

std::string nameFromValue(int value)
{
	//Get the name from the value
	switch (value)
	{
	case 11:
		return "Jack";
	case 12:
		return "Queen";
	case 13:
		return "King";
	case 14:
		return "Ace";
	default:
		return std::to_string(value);
	}
}

void shuffleCards(std::vector<Card*>* cards)
{
	std::random_device rd;
	std::default_random_engine rng(rd());
	std::ranges::shuffle(*cards, rng);
}

//-- CardUi --//

CardUi::CardUi(int x, int y, QWidget* parent) : QWidget(parent)
{
	setGeometry(x, y, 100, 140);
	setStyleSheet("background-color: rgb(200, 200, 255);");

	lblName = new QLabel("None", this);
	lblName->setStyleSheet("color: black; font-size: 18px; font-weight: bold");
	lblName->move(10, 10);
	lblName->resize(200, lblName->height());

	lblType = new QLabel("None", this);
	lblType->setStyleSheet("color: black; font-size: 18px; font-weight: bold");
	lblType->move(10, 40);
	lblType->resize(200, lblType->height());

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
	lblName->setText(QString::fromStdString(card->name));
	lblType->setText(QString::fromStdString(strFromType(card->type)));
}

//-- CardPair --//

CardPair::CardPair(Card* attack)
{
	this->attack = attack;
}

CardPair::CardPair(Card* attack, Card* defense)
{
	this->attack = attack;
	this->defense = defense;
}