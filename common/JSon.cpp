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

std::string jobj_encode(cJSON * _jobj)
{
	char * outStr= cJSON_PrintUnformatted(_jobj);
	std::string ret;
	if (outStr)
	{
		ret = std::string(outStr);
		free(outStr);
	}
	else
	{
		logger->log(Logger::LOG_ERROR,TAG_ENC ,"encode error");

	}
	return ret;
}
