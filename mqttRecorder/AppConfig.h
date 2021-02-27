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

class AppConfig {
public:
	AppConfig();
	virtual ~AppConfig();
	bool loadConfig(std::string filename);
	std::string mqtt_host;
	int mqtt_port;
	std::string mqtt_user;
	std::string mqtt_pwd;
	std::string rrd_sock;
	std::string rrd_db;
	std::vector <std::string> rec_topics;
	std::string qry_topic;

private:
	bool error;
	libconfig::Config cfg;
	void getString(std::string, std::string &);
	void getInteger(std::string, int &);
	void getVectorString(std::string name, std::vector <std::string> &value);

};

#endif /* MQTTRECORDER_APPCONFIG_H_ */
