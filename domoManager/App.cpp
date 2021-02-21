/*
 * App.cpp
 *
 *  Created on: 9 feb 2021
 *      Author: maurizio
 */
#include <vector>
#include "App.h"


#define DATA_SUBRSCRIPTION "/domotica/#"
#define QUERY_SUBRSCRIPTION "/query/req/#"

/*

{\"start\":\"now-1d\",\"end\":\"now\",\"step\":3600,\"tracks\":[{\"topic\":\"/domotica/test\",\"cf\":\"AVERAGE\"},{\"topic\":\"/domotica/test\",\"cf\":\"MAX\"}]}


 */



extern Logger * logger;

/*void App::data_topic_msg(std::string topic,std::string message, void * data)
{
	App * me = (App*) data;
	me->update_sample(topic,message);
}

void App::qry_topic_msg(std::string topic,std::string message,void * data)
{
	App * me = (App*) data;
	me->rrdb_query(topic,message);
}
void App::local_topic_msg(std::string topic,std::string message,void *data)
{
	App * me = (App*) data;
	printf("%s %s\n",topic.c_str(),message.c_str());
	if (topic == "/$LOCAL/mqtt/connection" && message == "connected")
	{
		me->_msgData = new Message(DATA_SUBRSCRIPTION,data_topic_msg,me);
		me->_msgQry = new Message(QUERY_SUBRSCRIPTION,qry_topic_msg,me);
	}
}*/

void App::msg_notify (std::string topic, std::string message)
{
	printf("****** %s %s****\n",topic.c_str(),message.c_str());
	if (topic == "local::/mqtt/connection" && message == "connected")
	{
		//mqtt_subscribe("/domotica/#");
	}
}

App::App():
		RRdb("/tmp/rrdcached.sock","/var/lib/rrdcached/db/"),
		MsgSrv("recorder","domotica", "domotica","localhost",1883)
{
	rrd_value_t * data=NULL;
	query_t qry;
	response_t resp;
	qry.start="now-1d";
	qry.end="now";
	qry.step=600;
	qry.tracks={{"_domotica_energia_main_P","AVERAGE"},{"_domotica_energia_main_P","MAX"}};
//	query(qry,resp);
//	JSonEncoder jresp;

//	printf("\n%s\n",jresp.response(resp).c_str());


	logger->log(Logger::LOG_VERBOSE,"App","ready");

	subscribe((MsgClient*)this,"local::/#");
	subscribe((MsgClient*)this,"/domotica/#");
	dispatch("local::/test","prova");
}

App::~App() {
	// TODO Auto-generated destructor stub
}
void App::rrdb_query(std::string topic,std::string sqry)
{
	query_t qry;
	response_t resp;
	std::string out;
	JSonDecoder jreq(sqry.c_str());
	if (jreq.error())
	{
		logger->log(Logger::LOG_ERROR,"APP","JSON parse error in %s",sqry.c_str());
		return;
	}
	jreq.query(qry);
	int samples = query(qry,resp);
	JSonEncoder jresp;
	out = jresp.response(resp);
	printf("\n%s\n",out.c_str());
}

int App::update_sample(string topicName, string payload)
{
	int ret;
	char* p;
	double num;
	num = strtod(payload.c_str(), &p);
	if (*p) {
		ret = -1;
	}
	else {
		if (update(topicName,num)==1)
		{
			if (create(topicName)!=0)
			{
				ret = -1;
			}
			else
			{
				ret = update(topicName,num);
			}
		}
	}
	return ret;
}
