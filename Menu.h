#pragma once

#include <QDialog>
#include "ui_Menu.h"

class Menu : public QDialog
{
	Q_OBJECT

public:
	Menu(QWidget *parent = nullptr);
	void pbConnect_Clicked();
	void pbStartServer_Clicked();
	void pbStartGame_Clicked();

private:
	Ui::MenuClass ui;
};
