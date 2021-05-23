#include "handlerHelp.h"

int clientHandle::getCode(std::string msg)
{
	return stoi(msg.substr(0, 3));
	
}

std::string clientHandle::createMsg(int code, std::string data)
{
	return std::to_string(code) + createSize(data.size()) + data;
}

std::string clientHandle::createSize(int size)
{
	std::string strSize = std::to_string(size);
	for (int i = SIZE_BYTES - strSize.size(); i != 0; i--)
	{
		strSize = "0" + strSize;
	}
	return strSize;
}

std::string clientHandle::getData(std::string msg)
{
	return msg.substr(6, msg.size() - 1);
}

std::string clientHandle::getIp(std::string msg)
{
	std::string ip = "";
	int i = 0;
	for (i ; msg[i] != ':'; i++);
	i++;
	for (i; msg[i] != ','; i++)
	{
		ip += msg[i];
	}
	return ip;
}

std::string clientHandle::getMac(std::string msg)
{
	std::string mac = "";
	int i = 0;
	for (i; msg[i] != ':'; i++);
	i++;
	for (i; msg[i] != ':'; i++);
	i++;
	for (i; msg[i] != ','; i++)
	{
		mac += msg[i];
	}
	return mac;
}

std::string clientHandle::getHostname(std::string msg)
{
	std::string hostname = "";
	int i = 0;
	for (i; msg[i] != ':'; i++);
	i++;
	for (i; msg[i] != ':'; i++);
	i++;
	for (i; msg[i] != ':'; i++);
	i++;
	for (i; msg[i] != ','; i++)
	{
		hostname += msg[i];
	}
	return hostname;
}

int clientHandle::getReputation(std::string msg)
{
	std::string reputation = "";
	int i = 0;
	for (i; msg[i] != ':'; i++);
	i++;
	for (i; msg[i] != ':'; i++);
	i++;
	for (i; msg[i] != ':'; i++);
	i++;
	for (i; msg[i] != ':'; i++);
	i++;
	for (i; i < msg.size() - 1 ; i++)
	{
		reputation += msg[i];
	}
	return stoi(reputation);
}
