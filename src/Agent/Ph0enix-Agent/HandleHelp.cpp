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
