#pragma once

#include <qwidget.h>
#include "Card.h"
#include "Player.h"

class CardUi : QWidget
{
public:
	QLabel* image;
	Card* card;
	CardUi(int x, int y, QWidget* parent = nullptr);
	~CardUi();
	void setCard(Card* card);
	std::function<void()> onClick;
	void mousePressEvent(QMouseEvent* event) override;
};

class HandUi : public QWidget
{
private:
	Player* player;
	std::vector<CardUi*> cards;
public:
	HandUi(QWidget* parent = nullptr);
	void setPlayer(Player* player);
	void refresh();
};

class AttackUi : public QWidget
{
public:
	std::vector<CardUi*> cardUis;
	AttackUi(QWidget* parent = nullptr);
	void refresh();
	void addAttack(Card* card, int index);
	void addDefense(Card* card, int index);
};

