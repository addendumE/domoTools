/*
 * Query.h
 *
 *  Created on: 18 feb 2021
 *      Author: maurizio
 */

#ifndef MQTTRECORDER_QUERY_H_
#define MQTTRECORDER_QUERY_H_

#include "RRdb.h"
#include "Jobj.h"
#include "MsgSys.h"
#include "Logger.h"

#include <string.h>


class Query: public MsgClient {
public:
	Query();
	virtual ~Query();
	bool start(std::string);
private:
	void msg_notify (std::string topic, std::string data);
	void rrdb_query(std::string topic,std::string sqry);
};

#endif /* MQTTRECORDER_QUERY_H_ */
