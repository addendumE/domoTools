/*
 * JSon.h
 *
 *  Created on: 10 feb 2021
 *      Author: maurizio
 */

#ifndef COMMON_JSON_H_
#define COMMON_JSON_H_

#include <cjson/cJSON.h>
#include <string>
#include <vector>

std::string jobj_encode(cJSON * _jobj);


class JSonDecoder{
public:
	JSonDecoder(std::string);
	virtual ~JSonDecoder();
	bool error();
	bool field(std::string tag, std::string &value);
	bool field(std::string tag, unsigned long &value);
	bool field(std::string array,std::string tag,int idx, std::string &value);

	bool arraySize(std::string tag, int &value);
protected:
	cJSON * object(std::string tag);
	cJSON *_jobj;
	bool	_error;
};


#endif /* COMMON_JSON_H_ */
