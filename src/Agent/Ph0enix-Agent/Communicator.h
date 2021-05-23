#pragma once
#include "stdafx.h"

#define _WINSOCK_DEPRECATED_NO_WARNINGS

class Communicator
{
public:
	Communicator(std::string ipAddr, int por);
	~Communicator();
	std::string recvMsg();
	void sendMessage(std::string message);
private:
	SOCKET serverSocket;
};

