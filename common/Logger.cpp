/*
 * Logger.cpp
 *
 *  Created on: 9 feb 2021
 *      Author: maurizio
 */
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>


#include "Logger.h"

Logger::Logger(std::string filename)
{
	// TODO Auto-generated constructor stub
}

Logger::~Logger() {
	// TODO Auto-generated destructor stub
}

void Logger::level(std::string tag, log_t level)
{

	_level[tag] = level;
}

void Logger::log(log_t level,std::string tag,std::string fmt,...)
{
	if (level<_level[tag]) return;
	time_t rawtime;
	time ( &rawtime );
	char *date = ctime (&rawtime);
	std::string sLevel;
	switch(level)
	{
		case LOG_VERBOSE: sLevel="VER";break;
		case LOG_DEBUG: sLevel="DBG";break;
		case LOG_ERROR: sLevel="ERR";break;
	}
	fmt = std::string(date,strlen(date)-1) +  " " + sLevel + " " + tag + " "  + " " + fmt + "\n";
	const char * cfmt=fmt.c_str();
	va_list argptr;
	va_start(argptr, cfmt);
	vfprintf(stdout, cfmt, argptr);
	va_end(argptr);
}


