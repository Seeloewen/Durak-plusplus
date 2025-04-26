#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Durak.h"
#include "Qlabel.h"
#include "qwidget.h"
#include "Player.h"
#include "vector"
#include "Card.h"
#include "qpushbutton.h"
#include "Widgets.h"
#include "qtextedit.h"

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
	void setDefender(int id);
	void setPlayer(Player* player);
	void setTrump(CardType type);
	void setPlayerStatus(PlayerStatus status);
	void writeAttackLog(std::string message);

	//Widgets
	HandUi* handUi;
	AttackUi* attackUi;
	QLabel* lblPlayerId;
	QLabel* lblCurrentDefender;
	QLabel* lblTrump;
	QLabel* lblPlayerStatus;
	QTextEdit* teAttackLog;
	QPushButton* btnLeaveAttack;
};


