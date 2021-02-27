/*
 * MsgSys.h
 *
 *  Created on: 17 feb 2021
 *      Author: maurizio
 */

#ifndef COMMON_MSGSYS_H_
#define COMMON_MSGSYS_H_

#include <string>
#include <vector>
#include <Mqtt.h>


class MsgClient
{
public:
	MsgClient() {};
	virtual ~MsgClient() {};
	virtual void msg_notify (std::string topic, std::string data) {};
};


class Subscription
{
public:
	Subscription(MsgClient *, std::string);
	virtual ~Subscription() {}
	std::string topic() {return _topic;}
	MsgClient * client() {return _client;}
private:
	MsgClient *_client;
	std::string _topic;
};



class MsgSrv: public Mqtt
{
public:
	MsgSrv(std::string cid);
	virtual ~MsgSrv();
	bool start(std::string user, std::string password, std::string host, int port);
	void subscribe(MsgClient *,std::string);
	void subscribe(MsgClient *client,std::vector <std::string> topics);
	void dispatch(std::string, std::string);
private:
	void mqtt_disconnected(int);
	void mqtt_connected(int);
	void mqtt_message(std::string topic, std::string message);
	std::vector <Subscription *> _subscriptions;
};

#endif /* COMMON_MSGSYS_H_ */
