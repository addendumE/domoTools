/*
 * History.cpp
 *
 *  Created on: 26 feb 2021
 *      Author: maurizio
 */

#include "History.h"
#include "Logger.h"
#include "JSon.h"

extern MsgSrv msgSrv;
extern Logger logger;

#define TAG "history"

void History::msg_notify (std::string topic, std::string message)
{
	buffer.push_back(stof(message.c_str()));
	if (buffer.size()>=_size)
	{
		buffer.erase(buffer.begin());
	}
	logger.log(Logger::LOG_VERBOSE,TAG,"msg %s %s",topic.c_str(),message.c_str());
	cJSON * jobj = cJSON_CreateFloatArray(&buffer[0],buffer.size());
	std::string str = jobj_encode(jobj);
	logger.log(Logger::LOG_VERBOSE,TAG,"%s",str.c_str());
	msgSrv.publish(_topic_out,str);
	cJSON_Delete(jobj);
}


History::History(std::string topic_in,std::string topic_out,int size) {
	_size = size;
	_topic_out = topic_out;
	msgSrv.subscribe((MsgClient*)this,topic_in);
	logger.log(Logger::LOG_VERBOSE,TAG,"history %s %d",topic_in.c_str(),size);
}

History::~History() {
	// TODO Auto-generated destructor stub
}

