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
	float tmp[ringBuff->size()];
	ringBuff->add(stof(message.c_str()));
	logger.log(Logger::LOG_VERBOSE,TAG,"msg %s %s",topic.c_str(),message.c_str());
	for(size_t i = 0; i < ringBuff->size(); ++i) {
			tmp[i]=ringBuff->get(i);
	}
	cJSON * jobj = cJSON_CreateFloatArray(tmp,ringBuff->size());
	cout << jobj_encode(jobj) << "\n";
	cJSON_Delete(jobj);
}


History::History(std::string topic_in,std::string topic_out,int size) {
	ringBuff = new RingBuffer <float>(size);
	msgSrv.subscribe((MsgClient*)this,topic_in);
	logger.log(Logger::LOG_VERBOSE,TAG,"history %s %d",topic_in.c_str(),size);
}

History::~History() {
	// TODO Auto-generated destructor stub
}

