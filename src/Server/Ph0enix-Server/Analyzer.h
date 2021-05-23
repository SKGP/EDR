#pragma once
#include "stdafx.h"

#define LOWEST_SCORE -80
#define MIN_DATA 15
/*
EDR analyzing system and can be helpful for sus commands, etc...
*/
class Analyzer {

public:
	Analyzer(std::vector<std::string> endpoints);
	~Analyzer();

	//public methods
	std::vector<std::pair<std::string, bool>> getResults();

private:
	std::vector<std::pair<std::string, int>> endpoints;

	//methods 
	bool searchForDeviation(std::string hostname);

};