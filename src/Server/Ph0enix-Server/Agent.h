#pragma once
#include "stdafx.h"
//linker error over here, dont add stdafx, TODO: fix st

class Agent
{
public:
	Agent();
	~Agent();
	std::string getIp();
	std::string getMac();
	std::string getHostName();
	int getReputation(); 
	SOCKET getsocket();
	bool getActive();
	void setIp(std::string ip);
	void setMac(std::string mac);
	void setHostName(std::string hostName);
	void setReputation(int reputation);
	void setSocket(SOCKET socket);
	void setActive(bool active);

	//score


private:
	std::string ip;
	std::string mac;
	std::string hostName;
	int reputation;
	SOCKET socket;
	bool active;
	//Score* _score;
};