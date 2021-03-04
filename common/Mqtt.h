/*
 * mqtt.h
 *
 *  Created on: 14 mag 2020
 *      Author: maurizio
 */

#ifndef MQTT_H_
#define MQTT_H_
#include <string>
#include <vector>
#include <unistd.h>

#include <mosquitto.h>
#include "Mqtt.h"

using namespace std;

class Mqtt
{

public:
	Mqtt(std::string);
	virtual ~Mqtt() {};
	void mqtt_connect(string user, string password,string host,int port);
	void mqtt_subscribe(std::string topic);
	void unsubscribe(string topic);
	void publish(string topic,string message, bool retain = true);
	bool match_sub(std::string topic,std::string sub);

	virtual void mqtt_message(std::string topic,std::string msg) {};
	virtual void mqtt_connected(int) {};
	virtual void mqtt_disconnected(int) {};
private:
	struct mosquitto * _client;
	static void message_cb(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *msg);
	static void connect_cb(struct mosquitto *mosq, void *userdata, int code);
	static void disconnect_cb(struct mosquitto *mosq, void *userdata, int code);
};



#endif /* MQTT_H_ */
