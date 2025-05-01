#include "Durak.h"
#include <QtWidgets/QApplication>
#include "Game.h"
#include "Menu.h"
#include "LogUtil.h"

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);

	initLog();
	showLog();

	logI(std::format("Durak++ v{} ({})", VERSION, VERDATE));

	Menu m;
	m.show();

	return a.exec();
}
