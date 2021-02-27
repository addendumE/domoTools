/*
 * Recorder.h
 *
 *  Created on: 18 feb 2021
 *      Author: maurizio
 */

#ifndef MQTTRECORDER_RECORDER_H_
#define MQTTRECORDER_RECORDER_H_

#include "RRdb.h"
#include "JSon.h"
#include "MsgSys.h"
#include "Logger.h"

#include <string>
#include <vector>


class Recorder: public MsgClient
{
public:
	Recorder();
	virtual ~Recorder();
	bool start(std::vector <std::string> rec_topics);

private:
	void msg_notify (std::string topic, std::string data);
	int update_sample(string topicName, string payload);
	void rrdb_query(std::string topic,std::string qry);
};

#endif /* MQTTRECORDER_RECORDER_H_ */
