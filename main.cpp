#include "Durak.h"
#include <QtWidgets/QApplication>
#include "Game.h"
#include "Menu.h"
#include "LogUtil.h"
#include <qprocess.h>

void handleLaunchArgs(QStringList args);

int main(int argc, char* argv[])
{
	app = new QApplication(argc, argv);

	initLog();
	showLog();
	logI(std::format("Durak++ v{} ({})", VERSION, VERDATE));

	handleLaunchArgs(QCoreApplication::arguments());

	Menu m;
	m.show();

	return app->exec();
}

void handleLaunchArgs(QStringList args)
{
	if (args.contains("-showLog")) {
		//Show log on startup	
		showLog();
	}
}
