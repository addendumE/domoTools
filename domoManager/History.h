/*
 * History.h
 *
 *  Created on: 26 feb 2021
 *      Author: maurizio
 */

#ifndef DOMOMANAGER_HISTORY_H_
#define DOMOMANAGER_HISTORY_H_

#include "MsgSys.h"
#include <string>

class History:MsgClient  {
public:
	History(std::string topic_in,std::string topic_out,int size);
	virtual ~History();
private:
	int _size;
	void msg_notify (std::string topic, std::string message);
	std::vector <float> buffer;
};

#endif /* DOMOMANAGER_HISTORY_H_ */
