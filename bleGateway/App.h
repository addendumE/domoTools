/*
 * App.h
 *
 *  Created on: 9 feb 2021
 *      Author: maurizio
 */

#ifndef BLEGATEWAY_APP_H_
#define BLEGATEWAY_APP_H_
#include "MsgSys.h"
#include "Logger.h"
#include "AppConfig.h"
#include "Eq3.h"

#include <time.h>


#include <string>
#include <vector>

class App: public MsgClient{
public:
	App(std::string);
	virtual ~App();
	void run(void);
private:
	void setScanMode(bool);
	void bleProcess(std::string addr, std::vector <unsigned char> data);
	void publish(string topic, float value);
	void msg_notify (std::string topic, std::string data);
	vector <Eq3 *> _eq3s;
	bool scanMode;
	time_t pauseTs;
};

#endif /* MQTTRECORDER_APP_H_ */
