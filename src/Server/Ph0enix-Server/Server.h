#pragma once
#include "stdafx.h"


class Server
{
public:
	Server();
	~Server();

	void serve(int port);
	void pingAllAgents();
	void printAllActive();

	void testAllWatchlist();

private:

	void accept();
	void clientHandler(SOCKET clientSocket);

	SOCKET _serverSocket;
	std::vector<Agent> agents;
};

