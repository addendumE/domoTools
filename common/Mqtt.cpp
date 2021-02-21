/*
 * mqtt.cpp
 *
 *  Created on: 14 mag 2020
 *      Author: maurizio
 */


#include <stdio.h>
#include <stdlib.h>

#include "mosquitto.h"
#include "Mqtt.h"
#include "Logger.h"

#define TAG "MQTT"

#define QOS         1

#include <string>
using namespace std;
extern Logger *logger;

void Mqtt::message_cb(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message)
{
	logger->log(Logger::LOG_VERBOSE,TAG,"message %s %s",message->topic,message->payload);
	std::string topicString(message->topic);
    std::string payloadString((char*)message->payload,(size_t)message->payloadlen);
    Mqtt* me = (Mqtt*)userdata;
    me->mqtt_message(topicString,payloadString);
}


void Mqtt::unsubscribe(string topic)
{
	int ret = mosquitto_unsubscribe(_client,NULL,topic.c_str());
	if (ret!= MOSQ_ERR_SUCCESS)
	{
		logger->log(Logger::LOG_ERROR,TAG,"unsubscribe error: %s",mosquitto_strerror(ret));
	}
	else
	{
		logger->log(Logger::LOG_ERROR,TAG,"unsubscribed %s",topic.c_str());
	}
}

void Mqtt::publish(string topic,string message)
{
	logger->log(Logger::LOG_DEBUG,TAG,"publish %s->%s",topic.c_str(),message.c_str());;
	int ret = mosquitto_publish(_client,NULL,topic.c_str(),message.length(),message.c_str(),2,false);
	if (ret == MOSQ_ERR_SUCCESS)
	{
		logger->log(Logger::LOG_VERBOSE,TAG,"published %s %s",topic.c_str(),message.c_str());
	}
	else
	{
		logger->log(Logger::LOG_ERROR,TAG,"publish error %s",mosquitto_strerror(ret));
	}
}

void Mqtt::mqtt_subscribe(std::string topic)
{
	int ret;
	ret = mosquitto_subscribe(_client,NULL,topic.c_str(),QOS);
	if (ret != MOSQ_ERR_SUCCESS)
	{
		logger->log(Logger::LOG_ERROR,TAG,"subscribe error %s",mosquitto_strerror(ret));
	}
	else
	{
		logger->log(Logger::LOG_VERBOSE,TAG,"subscribed %s",topic.c_str());
	}
}

void Mqtt::connect_cb (struct mosquitto * mosq, void * userdata, int code){
    Mqtt* me = (Mqtt*)userdata;
	logger->log(Logger::LOG_DEBUG,TAG,"connected %d",code);
    me->mqtt_connected(code);
}

void Mqtt::disconnect_cb (struct mosquitto * mosq, void * userdata, int code){
	Mqtt* me = (Mqtt*)userdata;
	logger->log(Logger::LOG_DEBUG,TAG,"disconnected %d",code);
	me->mqtt_disconnected(code);
}

void Mqtt::mqtt_connect(string user,string password,string host,int port)
{
	logger->log(Logger::LOG_DEBUG,TAG,"connecting %s:%d",host.c_str(),port);
	mosquitto_username_pw_set(_client,user.c_str(),password.c_str());
	mosquitto_connect_async(_client,host.c_str(),port,10);
}
bool Mqtt::match_sub(std::string topic,std::string sub)
{
	bool ret = false;
	mosquitto_topic_matches_sub(sub.c_str(),topic.c_str(),&ret);
	return ret;
}

Mqtt::Mqtt(std::string cid)
{
	logger->level(TAG,Logger::LOG_VERBOSE);
	mosquitto_lib_init();
	_client = mosquitto_new(cid.c_str(),false,this);
	if(_client)
	{
		mosquitto_connect_callback_set(_client,connect_cb);
		mosquitto_message_callback_set(_client,message_cb);
		mosquitto_disconnect_callback_set(_client,disconnect_cb);
		mosquitto_loop_start(_client);
	}
	else
	{
		logger->log(Logger::LOG_ERROR,TAG,"mosquitto_new failed");
	}
}
