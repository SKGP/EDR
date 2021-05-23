#pragma once
#include "stdafx.h"

/*
As you probebly know the agent should be both client and server
since its waiting for messages from the server and sending messages to the server 

*/

class Server
{
public:
	Server();
	~Server();
	void serve(int port);

	SOCKET getSocket();

private:

	void accept();
	void clientHandler(SOCKET clientSocket);

	SOCKET _serverSocket;
};

