/*
 * JSOBJ.h
 *
 *  Created on: 10 feb 2021
 *      Author: maurizio
 */

#ifndef JOBJ_H_
#define JOBJ_H_

#include <JSon.h>
#include <string>
#include <RRdb.h>

class RRdbDecoder: public JSonDecoder{
public:
	RRdbDecoder(std::string);
	virtual ~RRdbDecoder();
	bool query(RRdb::query_t &qry);
private:
};

//std::string encode_response(RRdb::response_t &resp);

#endif /* JOBJ_H_ */
