/*
 * Config.h
 *
 *  Created on: 24 feb 2021
 *      Author: maurizio
 */

#ifndef MQTTRECORDER_APPCONFIG_H_
#define MQTTRECORDER_APPCONFIG_H_

#include <string>
#include <vector>
#include <map>


#include <libconfig.h++>

enum ble_type {BLE_TH,BLE_EQ3,BLE_UNK};

class AppConfig {
public:
	struct  device_t
	{
		std::string topic;
		ble_type type;
	};
	using devices_t = std::map <std::string,device_t>;

	AppConfig();
	virtual ~AppConfig();
	bool loadConfig(std::string filename);
	std::string mqtt_host;
	int mqtt_port;
	std::string mqtt_user;
	std::string mqtt_pwd;
	std::string eq3_exec;
	devices_t devices;

private:
	bool error;
	libconfig::Config cfg;
	void getString(std::string, std::string &);
	void getInteger(std::string, int &);
	void getVectorString(std::string name, std::vector <std::string> &value);
	void getVectorDevices(std::string name, devices_t &value);


};

#endif /* MQTTRECORDER_APPCONFIG_H_ */
