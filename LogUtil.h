#pragma once

#include <vector>
#include <string>
#include <Log.h>

enum LogType
{
	Info,
	Warning,
	Error
};

inline Log* l;
inline std::vector<std::string> messages;

void initLog();
void logI(std::string message);
void logW(std::string message);
void logE(std::string message);
void logT(LogType type, std::string message);
void log(LogType type, std::string message);
void showLog();
void updateLog();