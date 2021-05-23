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
Function is suppose to add a new agent to the db
TODO:
check if the client is already inside the DB to avoid crashing

NOTE:
group_policy stay empty since the admin itself need to add the users to one
*/
void DataAccess::NewAgent(std::string ip, std::string mac, std::string hostName, int reputation, bool active)
{
	sql::Statement* stmt;
	try { //in case agent already registered OR any other exception that inserting wrong value could cause
		stmt = this->conn->createStatement();
		stmt->execute("USE ph0enix");
		stmt->execute("INSERT INTO agents(Hostname, ip, mac, reputation, active) VALUES (\"" + hostName + "\", \"" + ip + "\", \"" + mac + "\", " + std::to_string(reputation) + ", " + std::to_string(active) + ")"); //ugly values insert, TODO: change the way of inserting these strings
		delete stmt;
	}
	catch (...) { //nothing to check, since its only registering new agent the agent would know its not registered
		std::cout << "Agent " << hostName << " already registered" << std::endl;
	}
}


/*
The function change the status of specific agent
*/
void DataAccess::changeStatus(std::string hostName, bool status)
{
	sql::Statement* stmt;
	try { //in case agent already registered OR any other exception that inserting wrong value could cause
		stmt = this->conn->createStatement();
		stmt->execute("USE ph0enix");
		stmt->execute("UPDATE agents SET active = " + std::to_string(status) + " WHERE hostname = \"" + hostName + "\""); 
		delete stmt;
	}
	catch (...) { //nothing to check, since its only registering new agent the agent would know its not registered
		std::cout << "Couldnt change status for hostname " << hostName << std::endl;
	}
}


/*
Function add specific hash to the watchlist
*/
void DataAccess::addToWatchList(std::string hash)
{
	sql::Statement* stmt;
	try {
		stmt = this->conn->createStatement();
		stmt->execute("USE ph0enix");
		stmt->execute("UPDATE processes SET watchlist = 1 WHERE sha1 = \"" + hash + "\" ");
		delete stmt;
	}
	catch (...) {
		std::cout << "Couldnt add proccess " << hash << " to the watchlist";
	}
}

std::vector<std::string> DataAccess::getEndpointsWithHash(std::string SusSha1)
{
	sql::Statement* stmt;
	sql::ResultSet* res;
	std::vector<std::string> endpoints;

	try {
		stmt = this->conn->createStatement();
		stmt->execute("USE ph0enix");
		res = stmt->executeQuery("SELECT hostname FROM processes WHERE sha1 = \"" + SusSha1 + "\"");
		while (res->next())
			endpoints.push_back(std::string(res->getString("hostname")));
		delete res;
		delete stmt;

	}
	catch (...) {
		std::cout << "couldnt get endpoints with the hash: " << SusSha1 << std::endl;
	}
	return endpoints;
}

/*
Get the hostname as parameters and return the group policy of the agent
*/
std::string DataAccess::getAgentGroup(std::string hostname) {
	sql::Statement* stmt;
	sql::ResultSet* res;
	std::string group = "";

	try {
		stmt = this->conn->createStatement();
		stmt->execute("USE ph0enix");
		res = stmt->executeQuery("SELECT group_policy FROM agents WHERE Hostname = \"" + hostname + "\"");
		while (res->next()) {
			group = std::string(res->getString("group_policy"));
		}
		delete res;
		delete stmt;
	}
	catch (...) {
		std::cout << "couldnt catch group policy" << std::endl;
	}
	return group;
}

/*
Get the maximum value the group can reach - second test only
*/
int DataAccess::getGroupMaximumScore(std::string group) {
	sql::Statement* stmt;
	sql::ResultSet* res;
	int score = 0;

	try {
		stmt = this->conn->createStatement();
		stmt->execute("USE ph0enix");
		res = stmt->executeQuery("SELECT highest_score FROM group_policy WHERE name = \"" + group + "\""); 
		while (res->next()) {
			score = int(res->getInt(1));
		}
		delete res;
		delete stmt;
	}
	catch (...) {
		std::cout << "coudlnt get score of group: " << group << std::endl;
	}
	return score;
}

std::vector<std::string> DataAccess::getWatchlist() {

	sql::Statement* stmt;
	sql::ResultSet* res;
	std::vector<std::string> hashes;
	try {
		stmt = this->conn->createStatement();
		stmt->execute("USE ph0enix");
		res = stmt->executeQuery("SELECT DISTINCT sha1 FROM ph0enix.processes WHERE watchlist = 1;");
		while (res->next()) {
			hashes.push_back(std::string(res->getString("sha1")));
		}
		delete res;
		delete stmt;
	}
	catch (...) {
		std::cout << "coudlnt get back watchlists" << std::endl;
	}
	return hashes;
}


void DataAccess::getStats(std::string hostname, std::vector<double>* cpu_usage, std::vector<int>* timestemp, std::vector<int>* network_segments_in, std::vector<int>* network_segments_out, std::vector<int>* watchlist, std::vector<std::string>* current_time) {
	sql::Statement* stmt;
	sql::ResultSet* res;
	try {
		stmt = this->conn->createStatement();
		stmt->execute("USE ph0enix");
		res = stmt->executeQuery("SELECT * FROM stats WHERE Hostname = \"" + hostname + "\"");
		while (res->next()) {
			cpu_usage->push_back(double(res->getDouble(2)));
			watchlist->push_back(int(res->getInt(4)));
			timestemp->push_back(int(res->getInt(3)));
			network_segments_in->push_back(int(res->getInt(5)));
			network_segments_out->push_back(int(res->getInt(6)));
			current_time->push_back(std::string(res->getString("current_time")));
		}
		delete res;
		delete stmt;
	}
	catch (...) {
		std::cout << "couldnt catch stats " << std::endl;
	}
}