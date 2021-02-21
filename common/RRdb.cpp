/*
 * RRdb.cpp
 *
 *  Created on: 10 dic 2020
 *      Author: maurizio
 */

// mosquitto_pub -h 192.168.1.113 -u domotica -P domotica -p 1884 -t "/query/client/AVERAGE/now-1h/now/10m/domotica/comfort/Bagno/ambiente/temp" -m ""

#include <vector>
#define TEMPLATE "DS:value:GAUGE:15m:U:U RRA:AVERAGE:0.1:10m:1M RRA:AVERAGE:0.5:1h:10y RRA:MIN:0.5:1h:10y RRA:MAX:0.5:1h:10y"
#include "RRdb.h"
#include <rrd.h>
extern "C" {
#include <rrd_client.h>
}
#include "Logger.h"

extern Logger * logger;
#define TAG "RRDB"

RRdb::RRdb(string daemon,string dbFolder ):
		_dbFolder(dbFolder){
	int ret;
	logger->level(TAG,Logger::LOG_VERBOSE);
	ret = rrdc_connect(daemon.c_str());
	if (ret)
		logger->log(Logger::LOG_ERROR,TAG,"%s",rrd_get_error());
	else
		logger->log(Logger::LOG_DEBUG,TAG,"rrdcached connected");

}

RRdb::~RRdb() {
	// TODO Auto-generated destructor stub
}

string RRdb::topicToFile(string topic)
{
	    std::replace(topic.begin(), topic.end(), '/', '_');
	    return _dbFolder+topic;
}

int RRdb::create(string topic)
{
	int ret;
	vector <const char*> cmd;
	string filename = topicToFile(topic)+".rrdb";
	cmd.push_back("DS:value:GAUGE:15m:U:U");
	cmd.push_back("RRA:AVERAGE:0.5:10m:1w");
	cmd.push_back("RRA:AVERAGE:0.5:1h:1y");
	cmd.push_back("RRA:AVERAGE:0.5:1d:10y");
	cmd.push_back("RRA:MIN:0.5:1h:1y");
	cmd.push_back("RRA:MAX:0.5:1h:1y");
	cmd.push_back("RRA:MIN:0.5:1d:10y");
	cmd.push_back("RRA:MAX:0.5:1d:10y");

	ret = rrdc_create(filename.c_str(),1,946684800,1,cmd.size(),(const char**)&cmd[0]);
	if (ret)
	{
		logger->log(Logger::LOG_ERROR,TAG,"%s",rrd_get_error());
	}
	else
	{
		logger->log(Logger::LOG_DEBUG,TAG,"create %s",filename.c_str());
	}
	return ret;
}

int RRdb::update(string topic,double value)
{
	string filename = topicToFile(topic)+".rrdb";
	char valString[64];
	char * pnt[1];
	pnt[0]=valString;
	int ret;
	sprintf(valString,"N:%f",value);
	rrd_clear_error();
	ret = rrdc_update(filename.c_str(),1,(const char * const*)pnt);
	if (ret)
	{
		string errmsg(rrd_get_error());
		if (errmsg.find("No such file:") == string::npos)
		{
			logger->log(Logger::LOG_ERROR,TAG,"%s",rrd_get_error());
		}
		else
		{
			logger->log(Logger::LOG_DEBUG,TAG,"%s",rrd_get_error());
			ret=1;//ask for db creation
		}
	}
	else
	{
		logger->log(Logger::LOG_VERBOSE,TAG,"%s updated with %f",topic.c_str(),value);
	}
	return ret;
}


int RRdb::query(query_t &qry,response_t &resp)
{
	rrd_value_t *data;
	int ret = 0;
	int tCount = qry.tracks.size();
	logger->log(Logger::LOG_VERBOSE,TAG,"query from %s to %s step %ld",qry.start.c_str(),qry.end.c_str(),qry.step);
	int i;
	string filename[tCount];
	string xport[tCount];
	vector <const char*> cmd;
	cmd.push_back("xport");
	string sstart="-s" + string(qry.start);
	string send="-e" + string(qry.end);
	string sres="-S"+ to_string(qry.step);
	cmd.push_back(sres.c_str());
	cmd.push_back(sstart.c_str());
	cmd.push_back(send.c_str());
	i=0;
	for (int i=0; i<tCount;i++)
	{
		filename[i] = "DEF:val"+to_string(i)+"="+topicToFile(qry.tracks[i].topic)+".rrdb:value:"+qry.tracks[i].cf;
		xport[i] = "XPORT:val"+to_string(i);
		cmd.push_back(filename[i].c_str());
		cmd.push_back(xport[i].c_str());
	}

	char **ds_namv;
	time_t real_start,real_end;
	unsigned long real_step;
	unsigned long real_boh;
	int mah;
	ret = rrd_xport(
			cmd.size(),
			(char**)&cmd[0],
			&mah,
			&real_start,
			&real_end,
			&real_step,
			&real_boh,
			&ds_namv,
			&data);
	if (ret==0)
	{
		free((ds_namv)[0]);
		free(ds_namv);
		ret  = tCount * (real_end - real_start) / real_step;
		resp.start = real_start;
		resp.end = real_end;
		resp.step = real_step;
		for (int i=0; i<qry.tracks.size(); i++)
		{
			vector<float>* vect = new vector<float>;
			resp.data.push_back(vect);
		}
		rrd_value_t *pnt = data;
		for (int i=0; i<ret; i++)
		{
			resp.data[i%tCount]->push_back(*pnt++);
		}
		free(data);
	}
	else
	{
		logger->log(Logger::LOG_ERROR,TAG,"error: %s",rrd_get_error());
		ret = -1;
	}

	return ret;
}
