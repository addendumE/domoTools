/*
 * Logger.h
 *
 *  Created on: 9 feb 2021
 *      Author: maurizio
 */

#ifndef COMMON_LOGGER_H_
#define COMMON_LOGGER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>


class Logger {
public:
	enum log_t { LOG_VERBOSE = 1, LOG_DEBUG = 2, LOG_ERROR = 3 };
	Logger(std::string filename="");
	virtual ~Logger();
	void log(log_t level,std::string tag,std::string fmt,...);
	void level(std::string tag, log_t level);
private:
	std::map<std::string,int> _level;
	std::string _tag;
	FILE *_file;
};

#endif /* COMMON_LOGGER_H_ */
