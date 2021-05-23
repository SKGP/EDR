#include "Analyzer.h"

//in case of one endpoint only send vector with 1 endpoint
Analyzer::Analyzer(std::vector<std::string> endpoints)
{
	DataAccess db;
	std::pair<std::string, int> currentEndpoint;
	for (int i = 0; i < endpoints.size(); i++) //copy the vector's content to the local one
	{
		currentEndpoint.first = endpoints[i];
		currentEndpoint.second = db.getGroupMaximumScore(db.getAgentGroup(endpoints[i]));
		this->endpoints.push_back(currentEndpoint);
	}
		
}

Analyzer::~Analyzer()
{
}

/*
The function do the first and second checks for the endpoint
firstcheck - default check of the hostname's group compared the max score to the lowest acceptable score
second check - analyzing the stats that the agent collects
*/
std::vector<std::pair<std::string, bool>> Analyzer::getResults()
{
	std::vector<std::pair<std::string, bool>> results;
	std::pair<std::string, bool> currentEndpoint;

	//std::cout << this->endpoints[0] << "'s group: " << db.getAgentGroup(this->endpoints[0]) << std::endl;
	//std::cout << "score is: " << db.getGroupMaximumScore(db.getAgentGroup(this->endpoints[0])) << std::endl;


	for (auto& endpoint : this->endpoints) {
		//------------first check------------
		currentEndpoint.first = endpoint.first;
		if (endpoint.second < LOWEST_SCORE)
			currentEndpoint.second = true;
		else	 
			currentEndpoint.second = false;
		//------------second check------------
		if (!currentEndpoint.second) //if first check failed 
			currentEndpoint.second = searchForDeviation(currentEndpoint.first);

		results.push_back(currentEndpoint);
	}


	return results;
}


bool Analyzer::searchForDeviation(std::string hostname) {
	bool sus = false;
	std::cout << "-------second check-------\n" << hostname << std::endl;
	DataAccess db;
	std::vector<double>* cpu_usage = new std::vector<double>();
	std::vector<int>* timestemp = new std::vector<int>();
	std::vector<int>* network_segments_in = new std::vector<int>();
	std::vector<int>* network_segments_out = new std::vector<int>();
	std::vector<int>* watchlist = new std::vector<int>();
	std::vector<std::string>* current_time = new std::vector<std::string>();

	db.getStats(hostname, cpu_usage, timestemp, network_segments_in, network_segments_out, watchlist, current_time);

	//check if there is enough information to analyze
	if (cpu_usage->size() <= MIN_DATA && timestemp->size() <= MIN_DATA && network_segments_in->size() <= MIN_DATA && network_segments_out->size() <= MIN_DATA && watchlist->size() <= MIN_DATA && current_time->size() <= MIN_DATA) {
		std::cout << "NOT ENOUGH DATA TO ANALYSE" << std::endl;
		return false;
	}
	
	/*
	std::cout << "cpu: " << cpu_usage->at(0) << std::endl;
	std::cout << "timestemp: " << timestemp->at(0) << std::endl;
	std::cout << "network segments: " << network_segments->at(0) << std::endl;
	std::cout << "watchlist: " << watchlist->at(0) << std::endl;
	std::cout << "current time: " << current_time->at(0) << std::endl;
	*/

	//calculating avarage of cpu
	float avg = std::accumulate(cpu_usage->begin(), cpu_usage->end(), 0.0) / cpu_usage->size(); 


	/*
	3 sigma rule !!
	in order to calculate the anomaly we used 3 sigma rule dev
	https://en.wikipedia.org/wiki/68%E2%80%9395%E2%80%9399.7_rule
	*/
	int var = 0;
	int sd = 0;
	std::cout << "\n\n\n-------------- cpu_usage anomalyes --------------\n\n\n";
	for (auto& num : *cpu_usage) {
		var += (num - avg) * (num - avg);
	}
	var /= cpu_usage->size();
	sd = sqrt(var);

	for (auto& x : *cpu_usage) {
		if (abs(x - avg) > 3 * sd) {
			std::cout << "\n\n x: " << x << "\n\n";
			sus = true;
		}
	}

	//ugly code, opti it for later - just a test !
	avg = std::accumulate(watchlist->begin(), watchlist->end(), 0) / watchlist->size();
	std::cout << "\n\n\n-------------- watchlist anomalyes --------------\n\n\n";
	var = 0;
	sd = 0;
	for (auto& num : *watchlist) {
		var += (num - avg) * (num - avg);
	}
	var /= watchlist->size();
	sd = sqrt(var);

	for (auto& x : *watchlist) {
		if (abs(x - avg) > 3 * sd) {
			std::cout << "\n\n x: " << x << "\n\n";
			sus = true;
		}
	}

	/*
	get differences between the network in since its total per day
	so we gotta take the next record minus the current record to get the difference
	*/
	std::vector<int> network_in_diff;
	std::vector<int> network_out_diff;

	for (int x = 0; x < network_segments_in->size(); x++) {
		if (x + 1 < network_segments_in->size())
			network_in_diff.push_back(network_segments_in->at(x + 1) - network_segments_in->at(x));
	}

	for (int x = 0; x < network_segments_out->size(); x++) {
		if (x + 1 < network_segments_out->size())
			network_out_diff.push_back(network_segments_out->at(x + 1) - network_segments_out->at(x));
	}

	avg = std::accumulate(network_in_diff.begin(), network_in_diff.end(), 0) / network_in_diff.size();
	std::cout << "\n\n\n-------------- network in anomalyes --------------\n\n\n";
	var = 0;
	sd = 0;
	for (auto& num : network_in_diff) {
		var += (num - avg) * (num - avg);
	}
	var /= network_in_diff.size();
	sd = sqrt(var);

	for (auto& x : network_in_diff) {
		if (abs(x - avg) > 3 * sd) {
			std::cout << "\n\n x: " << x << "\n\n";
			sus = true;
		}
	}

	avg = std::accumulate(network_out_diff.begin(), network_out_diff.end(), 0) / network_out_diff.size();
	std::cout << "\n\n\n-------------- network out anomalyes --------------\n\n\n";
	var = 0;
	sd = 0;
	for (auto& num : network_out_diff) {
		var += (num - avg) * (num - avg);
	}
	var /= network_out_diff.size();
	sd = sqrt(var);

	for (auto& x : network_out_diff) {
		if (abs(x - avg) > 3 * sd) {
			std::cout << "\n\n x: " << x << "\n\n";
			sus = true;
		}
	}
	
	/*
	TODO:
	add network segment anomaly check
	*/

	//delete all the allocated vectors
	delete cpu_usage;
	delete timestemp;
	delete network_segments_in;
	delete network_segments_out;
	delete watchlist;
	delete current_time;
	return sus;
}