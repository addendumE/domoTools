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

App::App(std::string confFile)
{
	appConfig.loadConfig(confFile);
	msgSrv.start(appConfig.mqtt_user, appConfig.mqtt_pwd,appConfig.mqtt_host,appConfig.mqtt_port);
	msgSrv.subscribe((MsgClient*)this,"local::/#");
	logger.log(Logger::LOG_VERBOSE,TAG,"ready");
	pauseTs = 0;
	setScanMode(true);
}

void App::setScanMode(bool en)
{
	if (en && !scanMode)
	{
		ble_open();
		ble_start_scan();
		scanMode = true;
	}

	if (!en && scanMode)
	{
		ble_stop_scan();
		ble_close();
		scanMode = false;
	}
}

void App::msg_notify (std::string topic, std::string message)
{
	logger.log(Logger::LOG_VERBOSE,TAG,"msg %s %s",topic.c_str(),message.c_str());
	if (topic == "local::/mqtt/connection" && message == "connected")
	{
		if (_eq3s.size() == 0)
		{
			for (auto &item : appConfig.devices) // access by reference to avoid copying
			{
				if (item.second.type == BLE_EQ3)
				{
					_eq3s.push_back(new Eq3(item.first,item.second.topic,appConfig.eq3_exec));
				}
			}
		}
	}
	else  if (topic == "local::/ble/scan" && message == "pause")
	{
		setScanMode(false);
		pauseTs = time(NULL);
	}
}


void App::publish(string topic, float value)
{
	msgSrv.publish(topic,std::to_string(value));
}
void App::bleProcess(std::string addr, std::vector <unsigned char> data)
{
	auto it = appConfig.devices.find(addr);
	if (it == appConfig.devices.end()) return;
	std::string topic = it->second.topic;
	publish(topic+"rssi",(1.0f*(int8_t)data[data.size()-1]));
	//check if the device is a MIIJA T/H monitor
	if (it->second.type == BLE_TH && data[5]==0x95 && data[6]==0xFE && data.size()>=18){
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

void App::run(void)
{
	if (time(NULL) - pauseTs > 10 && !scanMode)
	{
		setScanMode(true);
	}

	if (scanMode)
	{
		std::string address,payload;
		vector <ble_adv_t> vmessages;

		ble_scan_loop(vmessages);
		for (auto & item : vmessages) {
			bleProcess(item.mac,item.data);
		}
	}
	else
	{
		for (auto &item : _eq3s) // access by reference to avoid copying
		{
			item->run();
		}

	}
}

App::~App() {
	// TODO Auto-generated destructor stub
}
