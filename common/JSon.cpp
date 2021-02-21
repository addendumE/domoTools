/*
 * JSon.cpp
 *
 *  Created on: 10 feb 2021
 *      Author: maurizio
 */

#include "JSon.h"
#include "Logger.h"
#include <float.h>
#include <math.h>

extern Logger *logger;

#define TAG_DEC "JSonDecoder"
#define TAG_ENC "JSonEncoder"


JSonDecoder::JSonDecoder(std::string txt) {
	logger->level(TAG_DEC,Logger::LOG_VERBOSE);
	_jobj = cJSON_Parse(txt.c_str());
	if (_jobj == NULL)
	{
		logger->log(Logger::LOG_ERROR,TAG_DEC ,"parse error in %s",txt.c_str());
		_error = true;
	}
	else
	{
		_error = false;
	}
}

cJSON * JSonDecoder::object(std::string tag)
{
	cJSON *jtmp;
	jtmp = cJSON_GetObjectItem(_jobj,tag.c_str());
	if (jtmp == NULL)
	{
		logger->log(Logger::LOG_ERROR,TAG_DEC ,"field %s not found",tag.c_str());
	}
	return jtmp;

}

bool JSonDecoder::field(std::string tag, std::string &value)
{
	cJSON *_jtmp;
	_jtmp = object(tag);
	if (_jtmp == NULL)
	{
		return false;
	}
	if (!cJSON_IsString(_jtmp))
	{
		logger->log(Logger::LOG_ERROR,TAG_DEC ,"field %s is not a string",tag.c_str());
		return false;
	}
	value = std::string(_jtmp->valuestring);
	return true;
}

bool JSonDecoder::field(std::string tag, unsigned long &value)
{
	cJSON *_jtmp;
	_jtmp = object(tag);
	if (_jtmp == NULL)
	{
		return false;
	}
	if (!cJSON_IsNumber(_jtmp))
	{
		logger->log(Logger::LOG_ERROR,TAG_DEC ,"field %s is not a string",tag.c_str());
		return false;
	}
	value = _jtmp->valueint;
	return true;
}

bool JSonDecoder::arraySize(std::string tag, int &value)
{
	cJSON *_jtmp;
	_jtmp = object(tag);
	if (_jtmp == NULL)
	{
		return false;
	}
	if (!cJSON_IsArray(_jtmp))
	{
		logger->log(Logger::LOG_ERROR,TAG_DEC ,"field %s is not an aray",tag.c_str());
		return false;
	}
	value = cJSON_GetArraySize(_jtmp);
	return true;
}


bool JSonDecoder::field(std::string array,std::string tag,int idx, std::string &value)
{
	cJSON *_jtmp;
	_jtmp = object(array);
	if (_jtmp == NULL)
	{
		return false;
	}
	if (!cJSON_IsArray(_jtmp))
	{
		logger->log(Logger::LOG_ERROR,TAG_DEC ,"field %s is not an aray",tag.c_str());
		return false;
	}
	if (idx>=cJSON_GetArraySize(_jtmp))
	{
		logger->log(Logger::LOG_ERROR,TAG_DEC ,"index bigger than array",tag.c_str());
		return false;
	}
	cJSON *_jtmp1 = cJSON_GetArrayItem(_jtmp, idx);
	cJSON *_jtmp2 = cJSON_GetObjectItem(_jtmp1,tag.c_str());
	value = std::string(_jtmp2->valuestring);

	return true;
}

bool JSonDecoder::error()
{
	return _error;
}

JSonDecoder::~JSonDecoder() {
	cJSON_Delete(_jobj);
}

bool JSonDecoder::query(RRdb::query_t &qry)
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


JSonEncoder::JSonEncoder() {
	logger->level(TAG_ENC,Logger::LOG_VERBOSE);
	_jobj = cJSON_CreateObject();
}

JSonEncoder::~JSonEncoder()
{
	cJSON_Delete(_jobj);
}

string JSonEncoder::response(RRdb::response_t &resp)
{
	cJSON_AddNumberToObject(_jobj,"start",resp.start);
	cJSON_AddNumberToObject(_jobj,"end",resp.end);
	cJSON_AddNumberToObject(_jobj,"step",resp.step);
	cJSON_AddNumberToObject(_jobj,"samples",resp.data[0]->size());

	double max[resp.data.size()];
	double min[resp.data.size()];

	struct cJSON * array = cJSON_CreateArray();
	cJSON_AddItemToObject(_jobj, "data",array);
	for (int i=0;i<resp.data.size();i++)
	{
		max[i]=FLT_MIN;
		min[i]=FLT_MAX;

		struct cJSON * tmp = cJSON_CreateArray();
		for (int m=0;m<resp.data[i]->size();m++)
		{
			double val = (*resp.data[i])[m];
			//double val = 30.0 * static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			struct cJSON * num = cJSON_CreateNumber(val);
			cJSON_AddItemToArray(tmp,num);
			if (!isnan(val) && num!=NULL)
			{
				if (val>max[i]) max[i]=val;
				if (val<min[i]) min[i]=val;
			}

		}
		cJSON_AddItemToArray(array,tmp);
	}
	cJSON_AddItemToObject(_jobj, "max",cJSON_CreateDoubleArray(max,resp.data.size()));
	cJSON_AddItemToObject(_jobj, "min",cJSON_CreateDoubleArray(min,resp.data.size()));


	//cJSON_AddItemToObject(jresp, "data",resp.dataarray);





	char * outStr= cJSON_PrintUnformatted(_jobj);
	std::string ret = string(outStr);
	free(outStr);
	return ret;
}

