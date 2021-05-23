#pragma once
#include "stdafx.h"

class VirusTotal
{
public:
	VirusTotal();
	~VirusTotal();

	bool checkUnknownProcesses(std::string sha1);
};

