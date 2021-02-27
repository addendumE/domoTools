/*
 * App.cpp
 *
 *  Created on: 9 feb 2021
 *      Author: maurizio
 */
#include <vector>
#include "App.h"

#include "AppConfig.h"
#include "History.h"
Logger logger("domoManager");
AppConfig appConfig;
MsgSrv msgSrv("domoManager");

#define TAG "APP"

void App::msg_notify (std::string topic, std::string message)
{
	logger.log(Logger::LOG_VERBOSE,TAG,"msg %s %s",topic.c_str(),message.c_str());
	if (topic == "local::/mqtt/connection" && message == "connected")
	{
		if (_histories.size()==0)
		{
			for (auto &item : appConfig.histories) // access by reference to avoid copying
				{
					_histories.push_back(new History(std::get<0>(item),std::get<1>(item),std::get<2>(item)));
				}
		}
	}
}

App::App(std::string confFile)
{
	appConfig.loadConfig(confFile);
	msgSrv.start(appConfig.mqtt_user, appConfig.mqtt_pwd,appConfig.mqtt_host,appConfig.mqtt_port);
	msgSrv.subscribe((MsgClient*)this,"local::/#");

}

App::~App() {
	// TODO Auto-generated destructor stub
}
