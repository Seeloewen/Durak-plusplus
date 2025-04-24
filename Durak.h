#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Durak.h"
#include "Qlabel.h"
#include "qwidget.h"
#include "Player.h"
#include "vector"
#include "Card.h"
#include "qpushbutton.h"

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

class Durak : public QMainWindow
{
	Q_OBJECT

public:
	Durak(QWidget* parent = nullptr);
	Ui::DurakClass ui;
	~Durak();
	void init();
	void tick();
	void timerStart();
	void btnLeaveAttack_Clicked();

	//Widgets
	HandUi* handUi;
	AttackUi* attackUi;
	QLabel* lblPlayerId;
	QLabel* lblCurrentDefender;
	QPushButton* btnLeaveAttack;
};


