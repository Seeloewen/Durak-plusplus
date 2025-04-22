#include <string>
#include "LogUtil.h"
#include <ctime>
#include <chrono>
#include <sstream>
#include <qwidget.h>
#include <Log.h>
#include <qstring.h>
#include <Game.h>

void initLog()
{
	l = new Log();
}

void logI(std::string message)
{
	//Short form for info log
	log(Info, message);
}

void logW(std::string message)
{
	//Short form for warning log
	log(Warning, message);
}

void logE(std::string message)
{
	//Short form for error log
	log(Error, message);
}

void logT(LogType type, std::string message)
{
	//Log message invoked on the gui thread for cross-thread logging
	QObject* d = durak;
	QMetaObject::invokeMethod(durak, [d, type, message]() {
		log(type, message);
		}, Qt::QueuedConnection);
}

void log(LogType type, std::string message)
{
	std::string stype;

	//Get string from type
	switch (type)
	{
	case Info:
		stype = "INFO";
		break;
	case Warning:
		stype = "WARNING";
		break;
	case Error:
		stype = "ERROR";
		break;
	}

	//Get current time
	std::time_t t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

	//Construct and log message
	message = std::format("[{}] [{}] {}", t, stype, message);
	messages.push_back(message);

	updateLog();
}

void showLog()
{
	l->show();
	updateLog();
}

void updateLog()
{
	//Clear the log textbox and append all messages
	l->ui.teLog->clear();
	for (std::string str : messages)
	{
		l->ui.teLog->append(QString::fromStdString(str));
	}
}