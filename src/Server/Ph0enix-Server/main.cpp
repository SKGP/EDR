#include "stdafx.h"
#include "Server.h"

#define PORT 42069



int main()
{

	//DataAccess db;
	//db.NewAgent("00.00.00.00", "00:00:00:00:00", "SK", 100, true);
	WSAInitializer wsaInit;
	Server myServer;
	
	try
	{
		std::thread server(&Server::serve, &myServer, PORT);
		server.detach();

		while (true) //ping all agents every 10 seconds to make sure they are online
		{
			myServer.testAllWatchlist();
			myServer.pingAllAgents();
			Sleep(5000);
			myServer.printAllActive();
			Sleep(5000);
		}
	}
	catch (std::exception& e)
	{
		std::cout << "Error occured: " << e.what() << std::endl;;
	}
	system("PAUSE");
	return 0;
}