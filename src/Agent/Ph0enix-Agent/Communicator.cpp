#include "Communicator.h"

Communicator::Communicator(std::string ipAddr, int port)
{
	this->serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //save the socket for later use

	struct sockaddr_in serv_addr;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
	serv_addr.sin_addr.s_addr = inet_addr(ipAddr.c_str());
	connect(this->serverSocket, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
}

Communicator::~Communicator()
{
}

void Communicator::sendMessage(std::string message)
{

	send(this->serverSocket, message.c_str(), 255, 0);

	/*
	Here split to 2 threads, one listen and one send every time of need
	
	char msg[255];

	while (strcmp(msg, "exit") != 0)
	{
		std::cin >> msg;
		send(serverSocket, msg, 255, 0);
	}
	*/
}

std::string Communicator::recvMsg()
{
	char msg[255] = "";
	recv(this->serverSocket, msg, 255, 0);
	return msg;
}