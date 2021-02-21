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
#include <RRdb.h>

class JSonDecoder{
public:
	JSonDecoder(std::string);
	virtual ~JSonDecoder();
	bool error();
	bool field(std::string tag, std::string &value);
	bool field(std::string tag, unsigned long &value);
	bool field(std::string array,std::string tag,int idx, std::string &value);

	bool arraySize(std::string tag, int &value);
	bool query(RRdb::query_t &qry);
private:
	cJSON * object(std::string tag);
	cJSON *_jobj;
	bool	_error;
};


class JSonEncoder{
public:
	JSonEncoder();
	virtual ~JSonEncoder();
	std::string response(RRdb::response_t &resp);
private:
	cJSON *_jobj;
};


#endif /* COMMON_JSON_H_ */
