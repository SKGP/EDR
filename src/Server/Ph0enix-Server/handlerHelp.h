#pragma once
#include "stdafx.h"
#define SIZE_BYTES 4

// functions that will help with handling the client
class clientHandle
{
public:
	static int getCode(std::string msg);
	static std::string createMsg(int code, std::string data);
	static std::string createSize(int size);
	static std::string getData(std::string msg);
	static std::string getIp(std::string msg);
	static std::string getMac(std::string msg);
	static std::string getHostname(std::string msg);
	static int getReputation(std::string msg);

};
