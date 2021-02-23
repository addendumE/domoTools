/*
 * RRdb.h
 *
 *  Created on: 10 dic 2020
 *      Author: maurizio
 */

#ifndef SRC_RRDB_H_
#define SRC_RRDB_H_
#include <time.h>
#include <rrd.h>
#include <string>
#include <algorithm>
#include <vector>
#include <time.h>

using namespace std;
class RRdb {
public:
	struct track_t {
	    std::string topic;
	    std::string cf;
	};
	struct query_t
	{
		std::string start;
		std::string end;
		unsigned long step;
		vector <track_t> tracks;
	};
	struct response_t
	{
		time_t start;
		time_t end;
		unsigned long step;
		unsigned int tracks;
		std::vector <double> data;
	};

	RRdb(std::string daemon,std::string dbfolder);
	virtual ~RRdb();

	int create(string);
	int update(string topic,double value);
	int bulk_update(string topic,long long cnt);
	int query(query_t &qry,response_t  &response);
private:
	std::string _dbFolder;
	string topicToFile(string);
};

#endif /* SRC_RRDB_H_ */
