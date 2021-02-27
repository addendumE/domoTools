/*
 * App.h
 *
 *  Created on: 9 feb 2021
 *      Author: maurizio
 */

#ifndef MQTTRECORDER_APP_H_
#define MQTTRECORDER_APP_H_
#include "RRdb.h"
#include "MsgSys.h"
#include "Logger.h"
#include "Recorder.h"
#include "Query.h"



#include <string.h>

class App: public MsgClient
{
public:
	App(std::string);
	virtual ~App();
private:
	void msg_notify (std::string topic, std::string data);
};

#endif /* MQTTRECORDER_APP_H_ */
