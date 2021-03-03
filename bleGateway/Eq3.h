/*
 * Eq3.h
 *
 *  Created on: 1 mar 2021
 *      Author: maurizio
 */

#ifndef DOMOMANAGER_EQ3_H_
#define DOMOMANAGER_EQ3_H_

#include <sys/prctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <fstream>
#include <signal.h>
#include <iostream>
#include <spawn.h>
#include "errno.h"
#include <sys/types.h>
#include <sys/wait.h>

#include "MsgSys.h"


class Eq3:MsgClient {
public:
	Eq3(std::string mac, std::string topic,std::string exec);
	virtual ~Eq3();

	struct data
	{
		float temperature;
		float valve;
		bool autoMode;
		bool manualMode;
		bool lowBatt;
		bool vacation;
		bool valid;
	};

	void run(void);

private:
	pid_t _pid;
	data _data;
	std::string _mac;
	std::string _topic;
	std::string _eq3_exec;
	std::string _fname;
	bool waitPid(pid_t,int &, bool);
	pid_t spawnCmd(char * cmd, char * argv[], char * fname=NULL);
	int eq3Cmd(std::string cmd);
	int decode(std::string);
	void msg_notify (std::string topic, std::string message);


};

#endif /* DOMOMANAGER_EQ3_H_ */
