#pragma once

#include <stdlib.h>
#include <string>
#include <qrect.h>
#include <qwidget.h>
#include <qlabel.h>
#include <functional>

enum CardType
{
	SPADES,
	DIAMONDS,
	HEART,
	CLUBS
};

class Card
{
public:
	CardType type;
	int value;
	std::string name;
	bool isTrump;
	int tablePos = -1;
	Card(CardType type, int value, std::string name);
};

class CardUi : QWidget
{
public:
	QLabel* lblName;
	QLabel* lblType;
	Card* card;
	CardUi(int x, int y, QWidget* parent = nullptr);
	~CardUi();
	void setCard(Card* card);
	std::function<void()> onClick;
	void mousePressEvent(QMouseEvent* event) override;
};

struct CardPair
{
	CardPair(Card* attack);
	CardPair(Card* attack, Card* defense);

	Card* attack = nullptr;
	Card* defense = nullptr;
};

std::string strFromType(CardType type);
std::string nameFromValue(int value);
void shuffleCards(std::vector<Card*>* cards);