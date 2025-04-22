#pragma once

#include <QWidget>
#include "ui_Log.h"

class Log : public QWidget
{
	Q_OBJECT

public:
	Log(QWidget *parent = nullptr);
	Ui::LogClass ui;
};
