#include "DataAccess.h"


DataAccess::DataAccess()
{
	sql::mysql::MySQL_Driver* driver;
	driver = sql::mysql::get_mysql_driver_instance();
	this->conn = driver->connect("tcp://127.0.0.1:3306", "USER", "PASSWORD");

}

DataAccess::~DataAccess()
{
	delete this->conn;
}

/*
The function counts the num of processes in the watch list
*/

int DataAccess::numOnWatchList(std::string hostName)
{
	sql::Statement* stmt;
	sql::PreparedStatement* pstmt;
	int processes = 0;
	try
	{
		stmt = this->conn->createStatement();
		stmt->execute("USE ph0enix");
		pstmt = conn->prepareStatement("SELECT COUNT(*) FROM ph0enix.processes WHERE hostname = \"" + hostName + "\" AND watchlist = 1");
		processes = int(pstmt->executeQuery());
		if (processes < 0 || processes > 1000)
		{
			processes = 0;
		}
		delete stmt;
		delete pstmt;
	}
	catch (...) {
		std::cout << "error occured while extarcting data from processes" << std::endl;
	}
	return processes;
}

/*
Function add data about the client to the DB
*/
void DataAccess::AgentData(std::string hostName)
{
	sql::Statement* stmt;
	std::vector<int> values = getTcpStatsA();
	try { //in case agent already registered OR any other exception that inserting wrong value could cause
		stmt = this->conn->createStatement();
		stmt->execute("USE ph0enix");
		stmt->execute("INSERT INTO stats(Hostname, cpu_usage, uptime, watchlist_processes, network_segments_in, network_segments_out, ph0enix.stats.current_time) VALUES (\"" + hostName + "\", " + std::to_string(checkCPU()) + ", " + std::to_string(upTimeInMin()) + ", " + std::to_string(numOnWatchList(hostName)) + ", " + std::to_string(values[0]) + ", " + std::to_string(values[1]) + ", \"" + timeNow() + "\"" + ")"); //ugly values insert, TODO: change the way of inserting these strings
		delete stmt;
	}
	catch (...) { //nothing to check, since its only registering new agent the agent would know its not registered
		std::cout << "error occured while data insert" << std::endl;
	}
}

void DataAccess::updateStats(std::string hostName)
{
	while (true)
	{
		AgentData(hostName);
		std::this_thread::sleep_for(std::chrono::seconds(5));
	}
}


