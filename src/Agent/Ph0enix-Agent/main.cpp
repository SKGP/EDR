#include "stdafx.h"
#include "HandlerHelp.h"
#include "msgCodes.h"
#define PORT 42069

void main()
{

	Sniffer sniff;
	DataAccess DB;
	std::thread sniffer(&Sniffer::Sniffing, &sniff);
	//int res = sniff.Sniffing();
	sniffer.detach();
	/*
	std::thread upTime(checkUpTime);
	//int res = sniff.Sniffing();
	upTime.detach();

	std::thread helpCpu(checkCPU);
	//int res = sniff.Sniffing();
	helpCpu.detach();
	*/
	std::string host = "nah";
	std::thread updateStats(&DataAccess::updateStats, DB, host);
	updateStats.detach();
	//register/login
	WSAInitializer wsaInit;
	Communicator com("127.0.0.1", 42069); //set up communicator to a server
	//com.sendMessage("main");
	Sleep(3000);
	std::cout << "2 threads are running yea" << std::endl;
	com.sendMessage(clientHandle::createMsg(SIGN_UP, "IP:127.0.0.1,Mac:66-66-66-66-66-66,Hostname:nah,reputation:-480"));
	//std::cout << clientHandle::getData(com.recvMsg()) << std::endl;

	/*
	TODO:
	1. build server handler
	2. if com.recvMsg gets ping msg return with pong
	3. server already know what to do when he get ping req so no need to worry about that case PING
	4. check why the server is crashing whenever client exit
	5. my guess that the server is crushing since its trying to listening to closed socket, try to find how to handle socket when program exit

	*/


	int code = 0;
	std::string data;
	std::string msg;
	while (true)
	{
		msg = com.recvMsg(); //get the message from the server
		code = clientHandle::getCode(msg.c_str());
		data = clientHandle::getData(msg.c_str());

		switch (code)
		{
		case PING:
			com.sendMessage(clientHandle::createMsg(PING, ""));
			break;
		default:
			std::cout << msg << std::endl;
			break;
		}


	}

	//here create a thread of recv message from the communicator


	Sleep(2000);
	com.sendMessage(clientHandle::createMsg(EXIT, "disconnect"));
	//std::cout << clientHandle::getData(com.recvMsg()) << std::endl;
	std::cout << "exit" << std::endl;
	//while (true);

	//for this stage the agent should only listen to the server without scanning, later on we will start to scan thing
	//the scanning part will be below 
}