/*
 * App.cpp
 *
 *  Created on: 9 feb 2021
 *      Author: maurizio
 */
#include <vector>
#include "App.h"

MsgSrv *msgSrv;
RRdb *rrDb;
Recorder *recorder = NULL;
Query *query = NULL;
Logger * logger;

#define TAG "APP"

/*

{\"start\":\"now-1d\",\"end\":\"now\",\"step\":3600,\"tracks\":[{\"topic\":\"/domotica/test\",\"cf\":\"AVERAGE\"},{\"topic\":\"/domotica/test\",\"cf\":\"MAX\"}]}


 */


void App::msg_notify (std::string topic, std::string message)
{
	logger->log(Logger::LOG_VERBOSE,TAG,"msg %s %s",topic.c_str(),message.c_str());
	if (topic == "local::/mqtt/connection" && message == "connected")
	{
		if (!recorder)
		{
			recorder = new Recorder();
		}
		if (!query)
		{
			query = new Query();
		}

	}
}

App::App()
{
	logger = new Logger("mqttRecorder");
	msgSrv = new MsgSrv("recorder","domotica", "domotica","localhost",1883);
	rrDb = new RRdb("/tmp/rrdcached.sock","/var/lib/rrdcached/db/");

	msgSrv->subscribe((MsgClient*)this,"local::/#");
}

App::~App() {
	// TODO Auto-generated destructor stub
}
