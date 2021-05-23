#pragma once
#include "stdafx.h"

class DataAccess
{
public:
	DataAccess(); //set up DB here
	~DataAccess();
	void NewAgent(std::string ip, std::string mac, std::string hostName, int reputation, bool active);
	void changeStatus(std::string hostName, bool status);
	std::vector<std::string> getWatchlist();
	void addToWatchList(std::string hash); //function gets only hash since it only needs to check the hash and set to all agents
	std::vector<std::string> getEndpointsWithHash(std::string SusSha1);
	std::string getAgentGroup(std::string hostname);
	int getGroupMaximumScore(std::string group);
	void getStats(std::string hostname, std::vector<double>* cpu_usage, std::vector<int>* timestemp, std::vector<int>* network_segments_in, std::vector<int>* network_segments_out, std::vector<int>* watchlist, std::vector<std::string>* current_time);
private:
	sql::Connection* conn;

};
