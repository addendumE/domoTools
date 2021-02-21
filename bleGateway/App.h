/*
 * App.h
 *
 *  Created on: 9 feb 2021
 *      Author: maurizio
 */

#ifndef BLEGATEWAY_APP_H_
#define BLEGATEWAY_APP_H_
#include "MsgSys.h"
#include "Logger.h"
#include "pthread.h"

#include <string>

class App: public MsgClient{
public:
	App();
	virtual ~App();
private:
	void bleProcess(std::string addr, std::string data);
	void publish(string topic, float value);
	void msg_notify (std::string topic, std::string data);
	static void *scanThread(void *);
	std::map <std::string,std::string> _devices;
	pthread_t _thread_id;
};

#endif /* MQTTRECORDER_APP_H_ */
