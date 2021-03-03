/*
 * Eq3.cpp
 *
 *  Created on: 1 mar 2021
 *      Author: maurizio
 */

#include "Eq3.h"
#include "JSon.h"
#include "Logger.h"

#include <libgen.h>
#include <error.h>
#include <string.h>

#include <fstream>
#include <iostream>
#include <sstream>

extern Logger logger;
extern MsgSrv msgSrv;

#define TAG "EQ3"

Eq3::Eq3(std::string mac, std::string topic, std::string exec):
		_mac(mac),
		_topic(topic),
		_eq3_exec(exec)
{
	msgSrv.subscribe((MsgClient*)this,_topic+"cmd");
	logger.log(Logger::LOG_VERBOSE,TAG,"new  %s %s",_topic.c_str(),exec.c_str());
	_data.valid = false;
	_pid = -1;
	_fname = "/tmp/";
	_fname = _fname + _mac.c_str() + ".json";
}

Eq3::~Eq3() {
}


/* spawn cmd as a new process
 * cmd: command with full path
 * argv: NULL terminated ary of arguments
 * fname: file name&path to capture the command output (NULL if not used)
 */
pid_t Eq3::spawnCmd(char * cmd, char * argv[], char * fname)
{
	pid_t pid = 0;
	int status;
	posix_spawn_file_actions_t * child_fd_actions = NULL;
	msgSrv.dispatch("local::/ble/scan","pause");
	logger.log(Logger::LOG_VERBOSE,TAG, "runCmd: %s", cmd);
	if (fname) {
		logger.log(Logger::LOG_VERBOSE,TAG,"runCmd: set out file to %s", fname);
		posix_spawn_file_actions_t child_fd_write_file;
		if (posix_spawn_file_actions_init(&child_fd_write_file)) {
			logger.log(Logger::LOG_VERBOSE,TAG,"runCmd: posix_spawn_file_actions_init error");
			return 0;
		}
		if (posix_spawn_file_actions_addopen(&child_fd_write_file,
				STDOUT_FILENO, fname, O_WRONLY | O_CREAT | O_TRUNC, 0644)) {
			logger.log(Logger::LOG_VERBOSE,TAG ,"runCmd: posix_spawn_file_actions_addopen");
			return 0;
		}
		child_fd_actions = &child_fd_write_file;
	}

	status = posix_spawn(&pid, cmd, child_fd_actions, NULL, argv, environ);
	if (status != 0) {
		logger.log(Logger::LOG_VERBOSE,TAG,"runCmd: posix_spawn error: %s\n",
				strerror(status));
	}
	logger.log(Logger::LOG_VERBOSE,TAG, "runCmd: posix_spawn pid is %d", pid);
	return pid;
}

bool Eq3::waitPid(pid_t pid, int &retCode, bool noBlock)
{
	// Wait for the child to quit so we don't leave a zombie
	int status;
	int ret = waitpid(pid, &status, (noBlock)?WNOHANG:0);
	if(ret>0)
	{
		retCode = WEXITSTATUS(status);
		logger.log(Logger::LOG_VERBOSE,TAG, "waitPid: pid %d return code is %d", pid,
			retCode);
	}
	return ret==0;
}

int Eq3::decode(std::string fname)
{
/*	msgSrv.dispatch(_topic+"/set",String(obj.temperature),{retain:true});
	msgSrv.dispatch(_topic+"/valvola",String(obj.valve),{retain:true});
	msgSrv.dispatch(_topic+"/auto",String((obj.auto) ? 1:0),{retain:true});
	msgSrv.dispatch(_topic+"/manual",String((obj.manual) ? 1:0),{retain:true});
	msgSrv.dispatch(_topic+"/lowBatt",String((obj['low battery']) ? 1:0),{retain:true});
	msgSrv.dispatch(_topic+"/openWind",String((obj['open window']) ? 1:0),{retain:true});
	msgSrv.dispatch(_topic+"/vacation",String((obj.vacation) ? 1:0),{retain:true});
	ue Mar  2 11:06:13 2021 VER EQ3  waitPid: pid 13746 return code is 0
{
  "temperature" : 23.0,
  "valve" : 11,
  "mode" : {
    "auto" : true,
    "manual" : false,
    "low battery" : false,
    "open window" : false,
    "vacation" : false,
    "locked" : true,
    "boost" : false,
    "unknown" : false,
    "dst" : true,
    "on" : false,
    "off" : false
  }
}
	*/
	std::string str;
	std::ifstream inFile;
	inFile.open(fname); //open the input file
	std::stringstream strStream;
	strStream << inFile.rdbuf(); //read the file
	str = strStream.str(); //str holds the content of the file
	JSonDecoder jdec(str);
	jdec.field("temperature",_data.temperature);
	jdec.field("valve",_data.valve);
	jdec.field("mode/auto",_data.autoMode);
	jdec.field("mode/manual",_data.manualMode);
	jdec.field("mode/low battery",_data.lowBatt);
	jdec.field("mode/vacation",_data.vacation);
	_data.valid = true;
	logger.log(Logger::LOG_VERBOSE,TAG,"%s",_topic.c_str());
	logger.log(Logger::LOG_VERBOSE,TAG,"  temperature:%f",_data.temperature);
	logger.log(Logger::LOG_VERBOSE,TAG,"  valve:%f",_data.valve);
	logger.log(Logger::LOG_VERBOSE,TAG,"  auto:%d",_data.autoMode);
	logger.log(Logger::LOG_VERBOSE,TAG,"  manual:%d",_data.manualMode);
	logger.log(Logger::LOG_VERBOSE,TAG,"  vacation:%d",_data.vacation);
	logger.log(Logger::LOG_VERBOSE,TAG,"  lowBattery:%d",_data.lowBatt);
	msgSrv.publish(_topic+"set",to_string(_data.temperature));
	msgSrv.publish(_topic+"valvola",to_string(_data.valve));
	msgSrv.publish(_topic+"auto",to_string(_data.autoMode));
	msgSrv.publish(_topic+"manual",to_string(_data.manualMode));
	msgSrv.publish(_topic+"lowBatt",to_string(_data.lowBatt));
	msgSrv.publish(_topic+"openWind",to_string(0));
	msgSrv.publish(_topic+"vacation",to_string(_data.vacation));
	return 1;
}

void Eq3::msg_notify (std::string topic, std::string message)
{
	logger.log(Logger::LOG_VERBOSE,TAG,"msg %s %s",topic.c_str(),message.c_str());
	eq3Cmd(message);
}

int Eq3::eq3Cmd(std::string cmd)
{
	int retCode=-1;
	_data.valid = false;
	logger.log(Logger::LOG_VERBOSE,TAG,"%s -> %s",_mac.c_str(),cmd.c_str());

	msgSrv.publish(_topic+"cmdResult","BUSY");//cmd pending
	const char * argv[]=
	{
		basename((char*)_eq3_exec.c_str()),
		_mac.c_str(),
		cmd.c_str(),
		NULL
	};
	_pid = spawnCmd((char*)_eq3_exec.c_str(),(char**)argv,(char*)_fname.c_str());
	return retCode;
}

void Eq3::run(void)
{
	int retCode;
	bool running;
	if (_pid>0){
		running = waitPid(_pid,retCode,true);
		if (running == 0)
		{
			_pid = 0;
			if (retCode == 0)
			{
				decode(_fname);
			}
			msgSrv.publish(_topic+"cmdResult",(retCode)? "ERR " + to_string(retCode) : "OK");//cmd pending

		}
	}
}
