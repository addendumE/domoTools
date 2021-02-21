/*
 * App.h
 *
 *  Created on: 9 feb 2021
 *      Author: maurizio
 */

#ifndef MQTTRECORDER_APP_H_
#define MQTTRECORDER_APP_H_
#include "RRdb.h"
#include "JSon.h"
#include "MsgSys.h"

#include "Logger.h"
#include <string.h>

class App: public RRdb , MsgClient, MsgSrv
{
public:
	App();
	virtual ~App();
private:
	void msg_notify (std::string topic, std::string data);

	int update_sample(string topicName, string payload);
	void rrdb_query(std::string topic,std::string qry);
	static void data_topic_msg(std::string topic,std::string message,void *data);
	static void qry_topic_msg(std::string topic,std::string message,void *data);
	static void local_topic_msg(std::string topic,std::string message,void *data);

};

#endif /* MQTTRECORDER_APP_H_ */
