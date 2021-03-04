/*
 * JSon.cpp
 *
 *  Created on: 10 feb 2021
 *      Author: maurizio
 */

#include "Jobj.h"

#include "Logger.h"
#include <float.h>
#include <math.h>

extern Logger *logger;

#define TAG_DEC "JobjDec"
#define TAG_ENC "JobjEnc"


RRdbDecoder::RRdbDecoder(std::string txt):
		JSonDecoder(txt) {
	logger->level(TAG_DEC,Logger::LOG_VERBOSE);
	if (_error)
	{
		logger->log(Logger::LOG_ERROR,TAG_DEC ,"parse error in %s",txt.c_str());
	}
}


RRdbDecoder::~RRdbDecoder() {
}

bool RRdbDecoder::query(RRdb::query_t &qry)
{
	field("start",qry.start);
	field("end",qry.end);
	field("step",qry.step);
	logger->log(Logger::LOG_DEBUG,TAG_DEC ,"query from %s to %s step %lu",
			qry.start.c_str(),
			qry.end.c_str(),
			qry.step
	);
	int i=0;
	arraySize("tracks",i);
	for (int n=0;n<i;n++)
	{
		std::string topic;
		std::string cf;
		field("tracks","topic",n,topic);
		field("tracks","cf",n,cf);
		RRdb::track_t track;
		track.topic=topic;
		track.cf=cf;
		qry.tracks.push_back(track);
	}
	return true;
}

/*
std::string encode_response(RRdb::response_t &resp)
{
	cJSON * jobj = cJSON_CreateObject();

	cJSON_AddNumberToObject(jobj,"start",resp.start);
	cJSON_AddNumberToObject(jobj,"end",resp.start);
	cJSON_AddNumberToObject(jobj,"step",resp.start);
	cJSON_AddNumberToObject(jobj,"samples",resp.data.size()/resp.tracks);


	struct cJSON * array = cJSON_CreateArray();

	double max[resp.tracks];
	double min[resp.tracks];
	struct cJSON * jdata[resp.tracks];
	for (int i = 0; i < resp.tracks; i++)
	{
		max[i] = -FLT_MAX;
		min[i] = FLT_MAX;
		jdata[i] = cJSON_CreateArray();
		cJSON_AddItemToArray(array,jdata[i]);
	}
	for (int m = 0; m < resp.data.size(); m++)
	{
		int track = m % resp.tracks;
		float val = resp.data[m];
		cJSON * jnum = cJSON_CreateNumber(val);
		cJSON_AddItemToArray(jdata[track],jnum);
		if (!isnan(val) && jnum!=NULL)
		{
			if (val>max[track]) max[track]=val;
			if (val<min[track]) min[track]=val;
		}
	}
	cJSON_AddItemToObject(jobj, "max",cJSON_CreateDoubleArray(max,resp.tracks));
	cJSON_AddItemToObject(jobj, "min",cJSON_CreateDoubleArray(min,resp.tracks));
	cJSON_AddItemToObject(jobj, "data",array);

	return jobj_encode(jobj);
}
*/
