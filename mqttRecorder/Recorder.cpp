/*
 * Recorder.cpp
 *
 *  Created on: 18 feb 2021
 *      Author: maurizio
 */

#include "Recorder.h"
#include <stdexcept>      // std::invalid_argument

extern MsgSrv msgSrv;
extern Logger logger;
extern RRdb	rrDb;

#define TAG "Recorder"

Recorder::Recorder() {

}

bool Recorder::start(std::vector <std::string> rec_topics)
{
	msgSrv.subscribe((MsgClient*)this,rec_topics);
	logger.log(Logger::LOG_VERBOSE,TAG,"ready");
	return true;
}

Recorder::~Recorder() {
	// TODO Auto-generated destructor stub
}

void Recorder::msg_notify (std::string topic, std::string message)
{
	logger.log(Logger::LOG_VERBOSE,TAG,"msg %s %s",topic.c_str(),message.c_str());
	update_sample(topic,message);
}

int Recorder::update_sample(string topicName, string payload)
{
	int ret;
	double num;
	try {
		num = stod(payload);
	}
	catch (const std::invalid_argument& ia) {
		  return -1;
	}

	if (rrDb.update(topicName,num)==1)
	{
		if (rrDb.create(topicName)!=0)
		{
			ret = -1;
		}
		else
		{
			ret = rrDb.update(topicName,num);
		}
	}
	return ret;
}
