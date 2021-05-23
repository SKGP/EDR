#pragma once
#include "stdafx.h"

class DataAccess
{
public:
	DataAccess(); //set up DB here
	~DataAccess();
	int numOnWatchList(std::string hostName);
	void AgentData(std::string hostName);
	void updateStats(std::string hostName);
private:
	sql::Connection* conn;

};