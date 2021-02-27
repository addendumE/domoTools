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


#include <libconfig.h++>

using history_t = std::tuple <std::string,std::string,int>;

class AppConfig {
public:
	AppConfig();
	virtual ~AppConfig();
	bool loadConfig(std::string filename);
	std::string mqtt_host;
	int mqtt_port;
	std::string mqtt_user;
	std::string mqtt_pwd;
	std::vector <history_t> histories;

private:
	bool error;
	libconfig::Config cfg;
	void getString(std::string, std::string &);
	void getInteger(std::string, int &);
	void getVectorHistory(std::string name, std::vector <history_t> &value);

};

#endif /* MQTTRECORDER_APPCONFIG_H_ */
