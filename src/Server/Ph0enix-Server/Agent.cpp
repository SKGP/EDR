#include "Agent.h"

Agent::Agent()
{
}

Agent::~Agent()
{
}

std::string Agent::getIp()
{
	return this->ip;
}

std::string Agent::getMac()
{
	return this->mac;
}

std::string Agent::getHostName()
{
	return this->hostName;
}

int Agent::getReputation()
{
	return this->reputation;
}

SOCKET Agent::getsocket()
{
	return this->socket;
}

bool Agent::getActive()
{
	return this->active;
}

void Agent::setIp(std::string ip)
{
	this->ip = ip;
}

void Agent::setMac(std::string mac)
{
	this->mac = mac;
}

void Agent::setHostName(std::string hostName)
{
	this->hostName = hostName;
}

void Agent::setReputation(int reputation)
{
	this->reputation = reputation;
}

void Agent::setSocket(SOCKET socket)
{
	this->socket = socket;
}

void Agent::setActive(bool active)
{
	this->active = active;
}
