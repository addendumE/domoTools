/*
 * App.cpp
 *
 *  Created on: 9 feb 2021
 *      Author: maurizio
 */
#include <vector>
#include "App.h"

#include "AppConfig.h"

Logger logger("mqttRecorder");
AppConfig appConfig;
MsgSrv msgSrv("mqttRecorder");
RRdb rrDb;
Recorder recorder;
Query query;


#define TAG "APP"

void App::msg_notify (std::string topic, std::string message)
{
	logger.log(Logger::LOG_VERBOSE,TAG,"msg %s %s",topic.c_str(),message.c_str());
	if (topic == "local::/mqtt/connection" && message == "connected")
	{
		recorder.start(appConfig.rec_topics);
		query.start(appConfig.qry_topic);
	}
}

App::App(std::string confFile)
{
	logger.level(TAG,Logger::LOG_ERROR);
	appConfig.loadConfig(confFile);
	msgSrv.start(appConfig.mqtt_user, appConfig.mqtt_pwd,appConfig.mqtt_host,appConfig.mqtt_port);
	rrDb.start(appConfig.rrd_sock,appConfig.rrd_db);
	msgSrv.subscribe((MsgClient*)this,"local::/#");
}

App::~App() {
	// TODO Auto-generated destructor stub
}
