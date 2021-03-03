/*
 * Config.cpp
 *
 *  Created on: 24 feb 2021
 *      Author: maurizio
 */

#include "AppConfig.h"
#include "Logger.h"
#include <iostream>
#include <iomanip>
#include <cstdlib>

extern Logger logger;

#define TAG "Config"

using namespace std;
using namespace libconfig;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


void test() {
    char* buffer;

    if( (buffer=getcwd(NULL, 0)) == NULL) {
        perror("failed to get current directory\n");
    } else {
        printf("%s \nLength: %zu\n", buffer, strlen(buffer));
        free(buffer);
    }

}

AppConfig::AppConfig() {
	error = false;
}

bool AppConfig::loadConfig(std::string filename) {
	test();
	 try
	   {
	     cfg.readFile(filename.c_str());
	   }
	   catch(const FileIOException &fioex)
	   {
		   logger.log(Logger::LOG_ERROR,TAG ,"error while reading file %s",filename.c_str());
	   }
	   catch(const ParseException &pex)
	   {
		 logger.log(Logger::LOG_ERROR,TAG ,"parsing error at line %d: %s",pex.getLine(),pex.getError());
	   }

	   getString("mqtt_host", mqtt_host);
	   getInteger("mqtt_port", mqtt_port);
	   getString("mqtt_user", mqtt_user);
	   getString("mqtt_pwd", mqtt_pwd);
	   getVectorHistory("history", histories);
	   return error;

}

void AppConfig::getVectorHistory(std::string name, std::vector <history_t> &value)
{
    try
    {
    	const Setting& root = cfg.getRoot();
    	const Setting &items = root[name];
    	for(int i = 0; i < items.getLength(); ++i)
    	{
    		std::string t_in = items[i].lookup("topic_in");
    		std::string t_out = items[i].lookup("topic_out");
    		int size = items[i].lookup("size");
    		value.push_back(std::make_tuple(t_in,t_out,size));
    		logger.log(Logger::LOG_DEBUG,TAG ,"%s[%d] is %s %s %d",name.c_str(),i,t_in.c_str(),t_out.c_str(),size);
    	}
    }
     catch(const SettingNotFoundException &nfex)
     {
	  logger.log(Logger::LOG_ERROR,TAG ,"%s not found",name.c_str());
	  error = true;
     }
}

void AppConfig::getString(std::string name, std::string &value)
{
    try
    {
    	 string tmp = cfg.lookup(name);
    	 value = tmp;
    	 logger.log(Logger::LOG_DEBUG,TAG ,"%s is  %s",name.c_str(),tmp.c_str());
    }
     catch(const SettingNotFoundException &nfex)
     {
	  logger.log(Logger::LOG_ERROR,TAG ,"%s not found",name.c_str());
	  error = true;
     }
}

void AppConfig::getInteger(std::string name, int &value)
{
    try
    {
       value = cfg.lookup(name);
     logger.log(Logger::LOG_DEBUG,TAG ,"%s is  %d",name.c_str(),value);
     }
     catch(const SettingNotFoundException &nfex)
     {
    	 logger.log(Logger::LOG_ERROR,TAG ,"%s not found",name.c_str());
    	 error = true;
     }
}

AppConfig::~AppConfig() {
	// TODO Auto-generated destructor stub
}

