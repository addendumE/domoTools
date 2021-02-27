/*
 * App.h
 *
 *  Created on: 9 feb 2021
 *      Author: maurizio
 */

#ifndef DOMOMANAGER_APP_H_
#define DOMOMANAGER_APP_H_
#include "MsgSys.h"
#include "Logger.h"
#include "History.h"


#include <string.h>


class App: public MsgClient
{
public:
	App(std::string);
	virtual ~App();
private:
	void msg_notify (std::string topic, std::string data);
	vector <History *> _histories;
};

#endif /* DOMOMANAGER_APP_H_ */
