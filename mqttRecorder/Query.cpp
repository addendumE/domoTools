/*
 * Query.cpp
 *
 *  Created on: 18 feb 2021
 *      Author: maurizio
 */

#include "Query.h"
#include "AppConfig.h"


extern MsgSrv msgSrv;
extern Logger logger;
extern RRdb	rrDb;

extern AppConfig appConfig;

#define TAG "Query"


Query::Query() {
	logger.level(TAG,Logger::LOG_ERROR);
}

Query::~Query() {
	// TODO Auto-generated destructor stub
}

bool Query::start(std::string topic)
{
	msgSrv.subscribe((MsgClient*)this,appConfig.qry_topic+"/req/#");
	logger.log(Logger::LOG_VERBOSE,TAG,"ready");
	return true;
}

void Query::msg_notify (std::string topic, std::string message)
{
	logger.log(Logger::LOG_VERBOSE,TAG,"msg %s %s",topic.c_str(),message.c_str());
	rrdb_query(topic,message);
}


void Query::rrdb_query(std::string topic,std::string sqry)
{
	RRdb::query_t qry;
	RRdb::response_t resp;
	RRdbDecoder jreq(sqry.c_str());
	if (jreq.error())
	{
		logger.log(Logger::LOG_ERROR,"APP","JSON parse error in %s",sqry.c_str());
		return;
	}
	jreq.query(qry);
	int ret = rrDb.query(qry,resp);
	if (ret == 0)
	{
		std::string out = encode_response(resp);
		std::string topicresp=topic.replace(0,11,"/query/resp/");
		msgSrv.publish(topicresp,out);
	}
}
