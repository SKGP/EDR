#include "Server.h"

Server::Server()
{

	// this server use TCP. that why SOCK_STREAM & IPPROTO_TCP
	// if the server use UDP we will use: SOCK_DGRAM & IPPROTO_UDP
	_serverSocket = socket(AF_INET,  SOCK_STREAM,  IPPROTO_TCP); 

	if (_serverSocket == INVALID_SOCKET)
		throw std::exception(__FUNCTION__ " - socket");



}

Server::~Server()
{
	try
	{
		// the only use of the destructor should be for freeing 
		// resources that was allocated in the constructor
		closesocket(_serverSocket);
	}
	catch (...) {}
}

void Server::serve(int port)
{
	
	struct sockaddr_in sa = { 0 };
	
	sa.sin_port = htons(port); // port that server will listen for
	sa.sin_family = AF_INET;   // must be AF_INET
	sa.sin_addr.s_addr = INADDR_ANY;    // when there are few ip's for the machine. We will use always "INADDR_ANY"

	// again stepping out to the global namespace
	// Connects between the socket and the configuration (port and etc..)
	if (bind(_serverSocket, (struct sockaddr*)&sa, sizeof(sa)) == SOCKET_ERROR)
		throw std::exception(__FUNCTION__ " - bind");
	
	// Start listening for incoming requests of clients
	if (listen(_serverSocket, SOMAXCONN) == SOCKET_ERROR)
		throw std::exception(__FUNCTION__ " - listen");
	std::cout << "Listening on port " << port << std::endl;

	while (true)
	{
		// the main thread is only accepting clients 
		// and add then to the list of handlers
		std::cout << "Waiting for client connection request" << std::endl;
		accept();
	}
}

void Server::accept()
{
	// notice that we step out to the global namespace
	// for the resolution of the function accept
	// this accepts the client and create a specific socket from server to this client
	SOCKET client_socket = ::accept(_serverSocket, NULL, NULL);

	if (client_socket == INVALID_SOCKET)
		throw std::exception(__FUNCTION__);

	std::cout << "Client accepted. Server and client can speak" << std::endl;

	// the function that handle the conversation with the client
	std::thread t1 (&Server::clientHandler,this , client_socket);
	t1.detach();
}


void Server::clientHandler(SOCKET clientSocket)
{
	DataAccess db;
	std::cout << "Agent connected" << std::endl;
	char msg[255] = "a";
	int code = 0;
	std::string data;
	std::string response;
	std::string ip;
	Agent agent;
	bool isLoggedIn = false;


	while (strcmp(msg, "exit") != 0)
	{
		strcpy(msg, "");
		recv(clientSocket, msg, 255, 0);
		std::cout << "msg: " << msg << std::endl;

		/*
		since the agent can unexpectedly exit we dont wanna insert nothing to getCode(msg) 
		so catch block will catch it and close the socket
		*/
		try {
			code = clientHandle::getCode(msg);
			data = clientHandle::getData(msg);
		}
		catch (...)
		{
			std::cout << "couldnt understand agent's message" << std::endl;
			strcpy(msg, "exit"); //close the socket since the agent exited
			break; //exit the loop
		}
		switch (code)
		{
			case SIGN_UP:
				//if(!hostname in vector) sign up / else agent where hostname == hostname active = 1
				isLoggedIn = false;
				for (auto& value : agents)
				{
					if (clientHandle::getHostname(data) == value.getHostName()) //check if the agent already in the vector
					{
						value.setActive(true); //lazy login, set its active status to active instead of creating new one ehhh
						value.setSocket(clientSocket); //set the socket to the new one !
						response = clientHandle::createMsg(SIGNED_UP_SUCCSESFULY, "Agent logged in");
						isLoggedIn = true;
						break; //exit since the agent already registered
					}
				}
				
				//in both ways we need to set up the agent locally
				agent.setSocket(clientSocket); 
				agent.setIp(clientHandle::getIp(data));
				agent.setMac(clientHandle::getMac(data));
				agent.setHostName(clientHandle::getHostname(data));
				agent.setReputation(clientHandle::getReputation(data));
				agent.setActive(true); //always true when sign up

				if (!isLoggedIn) //in case register
				{
					std::cout << "ip: " << agent.getIp() << "\nmac: " << agent.getMac() << "\nhostName: " << agent.getHostName() << "\nreputation: " << std::to_string(agent.getReputation()) << std::endl;
					agents.push_back(agent);
					db.NewAgent(agent.getIp(), agent.getMac(), agent.getHostName(), agent.getReputation(), agent.getActive());
					response = clientHandle::createMsg(SIGNED_UP_SUCCSESFULY, "Agent registered in the system");
				}
				else
				{
					db.changeStatus(agent.getHostName(), true);
				}
				break;


			case UPDATE_STATUS:
				//updating the staus of the agent
				ip = agent.getIp();
				for (int i = 0; i < this->agents.size(); i++)
				{
					if (ip == this->agents[i].getIp())
					{
						if (data == "Active")
						{
							this->agents[i].setActive(true);
						}
						else
						{
							this->agents[i].setActive(false);
						}
					}
				}
				response = clientHandle::createMsg(STATUS_UPDATED, "Status succsesfuly updated");
				break;
			case EXIT:
				response = clientHandle::createMsg(DISCONNECTED_SUCCSESFULY, "Agent disconnected");
				strcpy(msg, "exit"); //make sure to exit 
				break;
			case PING: //change the status to active since it answered
				for (auto& value : agents)
				{
					if (agent.getHostName() == value.getHostName()) //get the correct ip
					{
						value.setActive(true);
						db.changeStatus(value.getHostName(), true);
					}
				}
				break;
			default:
				response = clientHandle::createMsg(CODE_ERROR, "This msg code does not exist ");
				break;
		}
		send(clientSocket, response.c_str() , response.size() , 0);
	}
			

}

void Server::pingAllAgents()
{
	std::string response;
	DataAccess db;
	for (auto &value : agents)
	{
		db.changeStatus(value.getHostName(), false); //change value inside the DB
		response = clientHandle::createMsg(PING, "ping message");
		send(value.getsocket(), response.c_str(), response.size(), 0);
		value.setActive(false); //set all agents to not active since we need to know which one answer and active
	}
}


void Server::printAllActive()
{
	std::cout << "-----active------" << std::endl;
	for (auto& value : agents)
	{
		if (value.getActive())
			std::cout << value.getHostName() << std::endl;
	}
}

/*
The function gets a sha1 of a sus processs and send it to the analyzer, the analyzer decide
wethear pump an alart or not
main function of the analyzing process basically

results: <"hostname", "positive/negetive">
*/
void Server::testAllWatchlist() {
	DataAccess db;
	VirusTotal vt;
	std::vector<std::string> endpoints;
	std::vector<std::pair<std::string, bool>> results; //results will get back as the hostname and true for infected false for not
	std::vector<std::string> watchlist_hashes = db.getWatchlist();
	bool isOneStationChecked = false;
	for (auto& hash : watchlist_hashes) {
		std::cout << "---------------" << hash << "------------" << std::endl; //for debugging
		endpoints = db.getEndpointsWithHash(hash); //insert into the endpoints vector all the endpoints with the hash
		Analyzer analyzer(endpoints);
		results = analyzer.getResults(); //get back the results from the analyzer
		std::cout << "---------final results-------- - " << std::endl;
		for (auto& res : results) {
			std::cout << "Results of process: " << res.first << " : " << res.second << std::endl;
			if (res.second && !isOneStationChecked) { //at least 1 watchlist station got positive send to VT
				isOneStationChecked = true;
				if (vt.checkUnknownProcesses(hash)) {
					std::cout << "\n\n\nALERT ALERT ALERT !!!!!\n\n\n";
					std::cout << "-----------infected endpoints-----------\n\n\n";
					for (auto& endpoint : endpoints) {
						std::cout << endpoint << std::endl;
					}
				}
			}
		}
		isOneStationChecked = false;
		//the results here are the last results !! the VT check is inside the analyzer !
	}
	

}