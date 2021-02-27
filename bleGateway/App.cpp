/*
 * App.cpp
 *
 *  Created on: 9 feb 2021
 *      Author: maurizio
 */
#include "App.h"
#include "ble.h"
Logger logger("bleGateway");
AppConfig appConfig;
MsgSrv msgSrv("bleGateway");

#define TAG "APP"

App::App(std::string confFile):
		_thread_id(0)
{
	appConfig.loadConfig(confFile);
	msgSrv.start(appConfig.mqtt_user, appConfig.mqtt_pwd,appConfig.mqtt_host,appConfig.mqtt_port);
	msgSrv.subscribe((MsgClient*)this,"local::/#");
	logger.log(Logger::LOG_VERBOSE,TAG,"ready");
}

void App::msg_notify (std::string topic, std::string message)
{
	logger.log(Logger::LOG_VERBOSE,TAG,"msg %s %s",topic.c_str(),message.c_str());
	if (topic == "local::/mqtt/connection" && message == "connected")
	{
		if (!_thread_id)
		{
			int ret = pthread_create(&_thread_id,NULL,scanThread,this);
			if (ret == 0) {
				logger.log(Logger::LOG_VERBOSE,TAG,"BLE thread started");
			}
			else
			{
				logger.log(Logger::LOG_ERROR,TAG,"BLE thread creation error %d",ret);
			}
		}
	}
}


void App::publish(string topic, float value)
{
	msgSrv.publish(topic,std::to_string(value));
}
void App::bleProcess(std::string addr, std::string data)
{
	auto it = appConfig.devices.find(addr);
	if (it == appConfig.devices.end()) return;
	std::string topic = it->second;
	publish(topic+"rssi",(1.0f*(int8_t)data[data.size()-1]));
	//check if the device is a MIIJA T/H monitor
	if (data[5]==0x95 && data[6]==0xFE && data.size()>=18){
		switch(data[0x12])
		{
			case 0x0d:
				publish (topic+"temp",((data[0x15])+(data[0x16]<<8))/10.0f);
				publish (topic+"hum",((data[0x17])+(data[0x18]<<8))/10.0f);
				break;
			case 0x0a:
				publish (topic+"batt",data[0x15]);
				break;
			case 0x04:
				publish (topic+"temp",((data[0x15])+(data[0x16]<<8))/10.0f);
				break;
			case 0x06:
				publish (topic+"hum",((data[0x15])+(data[0x16]<<8))/10.0f);
				break;
		}
	}
}

void * App::scanThread(void * data)
{
	App * me= (App *) data;
	ble_open();
	ble_start_scan();
	std::string address,payload;
	while(1)
	{
		vector <ble_adv_t> vmessages;
		ble_scan_loop(vmessages);
		for (auto & item : vmessages) {
			me->bleProcess(item.mac,item.data);
		}
	}
	ble_stop_scan();
	ble_close();

	return NULL;
}

App::~App() {
	// TODO Auto-generated destructor stub
}
