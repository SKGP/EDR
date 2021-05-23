#pragma once
#include "stdafx.h"

class Score{

public:
	Score(std::string hostname);
	~Score();

private:
	int _score;
	std::string _hostname;
};