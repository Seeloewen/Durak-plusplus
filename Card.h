#pragma once

#include <stdlib.h>
#include <string>
#include <qrect.h>
#include <qwidget.h>
#include <qlabel.h>
#include <functional>

enum CardType
{
	PIK,
	KARO,
	HEART,
	CROSS
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

std::string strFromType(CardType type);
void shuffleCards(std::vector<Card*>* cards);

struct CardPair
{
	CardPair(Card* attack);
	CardPair(Card* attack, Card* defense);

	Card* attack = nullptr;
	Card* defense = nullptr;
};
