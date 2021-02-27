/*
 * MsgSys.cpp
 *
 *  Created on: 17 feb 2021
 *      Author: maurizio
 */

#include "MsgSys.h"

#include "mosquitto.h"

Subscription::Subscription(MsgClient *client, std::string topic):
	_topic(topic),
	_client(client)
{

}

MsgSrv::MsgSrv(std::string cid):
		Mqtt(cid)
{
}

bool MsgSrv::start(std::string user, std::string password, std::string host, int port)
{
	mqtt_connect(user,password,host,port);
	return true;
}


void MsgSrv::mqtt_disconnected(int code)
{
	dispatch("local::/mqtt/connection","disconnected");
}

void MsgSrv::mqtt_connected(int code)
{
	dispatch("local::/mqtt/connection","connected");
}

void MsgSrv::mqtt_message(std::string topic, std::string message)
{
	dispatch(topic,message);
}


MsgSrv::~MsgSrv()
{
	for (auto &sub : _subscriptions) // access by reference to avoid copying
	{
	    delete (sub);
	}
}

void MsgSrv::dispatch(std::string topic, std::string message)
{
	for (auto &sub : _subscriptions) // access by reference to avoid copying
	{
		bool ret;
		if (mosquitto_topic_matches_sub(sub->topic().c_str(),topic.c_str(),&ret)==MOSQ_ERR_SUCCESS)
		{
			if (ret)
			{
				sub->client()->msg_notify(topic,message);
			}
		}
	}
}

void MsgSrv::subscribe(MsgClient *client,std::string topic)
{
	Subscription *sub = new Subscription(client,topic);
	_subscriptions.push_back(sub);
	if (topic.find("local::") != 0 )
	{
		mqtt_subscribe(topic);
	}
}

void MsgSrv::subscribe(MsgClient *client,std::vector <std::string> topics)
{
	for (auto &topic : topics)
	{
		std::string t = topic;
		subscribe(client,t);
	}
}
