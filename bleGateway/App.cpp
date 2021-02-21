/*
 * App.cpp
 *
 *  Created on: 9 feb 2021
 *      Author: maurizio
 */
#include "App.h"
#include "ble.h"
Logger * logger;
MsgSrv *msgSrv;

#define TAG "APP"


App::App():
		_thread_id(0)
{
	logger =new Logger("bleGateway");
	msgSrv = new MsgSrv("bleGateway","domotica", "domotica","localhost",1883);
	_devices["4C:65:A8:DD:E6:E2"] = "/domotica/comfort/Bimbi/ambiente/";
	_devices["4C:65:A8:DD:DF:FE"] = "/domotica/comfort/Bagno/ambiente/";
	_devices["4C:65:A8:DD:DD:AC"] = "/domotica/comfort/Tinello/ambiente/";
	_devices["4C:65:A8:DD:D6:B1"] = "/domotica/comfort/Letto/ambiente/";
	_devices["58:2D:34:34:52:74"] = "/domotica/comfort/Corridoio/ambiente/";
	_devices["00:1A:22:0E:DB:1D"] = "/domotica/comfort/Bimbi/termosifone/";
	_devices["00:1A:22:0E:D3:B1"] = "/domotica/comfort/Bagno/termosifone/";
	_devices["00:1A:22:0E:DB:2C"] = "/domotica/comfort/Tinello/termosifone/";
	_devices["FA:39:B8:01:FB:71"] = "/domotica/debug/mi/";
	msgSrv->subscribe((MsgClient*)this,"local::/#");
	logger->log(Logger::LOG_VERBOSE,TAG,"ready");
}

void App::msg_notify (std::string topic, std::string message)
{
	logger->log(Logger::LOG_VERBOSE,TAG,"msg %s %s",topic.c_str(),message.c_str());
	if (topic == "local::/mqtt/connection" && message == "connected")
	{
		if (!_thread_id)
		{
			int ret = pthread_create(&_thread_id,NULL,scanThread,this);
			if (ret == 0) {
				logger->log(Logger::LOG_VERBOSE,TAG,"BLE thread started");
			}
			else
			{
				logger->log(Logger::LOG_ERROR,TAG,"BLE thread creation error %d",ret);
			}
		}
	}
}


void App::publish(string topic, float value)
{
	msgSrv->publish(topic,std::to_string(value));
}
void App::bleProcess(std::string addr, std::string data)
{
	std::string topic = _devices[addr];
	if ( topic != "")
	{
		publish(topic+"rssi",(1.0f*(int8_t)data[data.size()-1]));
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
}

void * App::scanThread(void * data)
{
	App * me= (App *) data;
	ble_open();
	ble_start_scan();
	std::string address,payload;
	while(1)
	{
		address="";
		ble_scan_loop(address,payload);
		if (address != "")
		{
			//logger->log(Logger::LOG_VERBOSE,TAG,"adv from %s len %d",address.c_str(),payload.size());
			me->bleProcess(address,payload);
		}
	}
	ble_stop_scan();
	ble_close();

	return NULL;
}


App::~App() {
	// TODO Auto-generated destructor stub
}
