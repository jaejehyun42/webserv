#include "Client.hpp"

Client::Client() : _port(0), _index(0)
{}

Client::Client(int port, int index, const string& ip) : _port(port), _index(index), _ip(ip)
{}

Client::~Client() {}

void Client::setMessage(const string& message)
{
	if (message == "")
		_message = "";
	else
		_message += message;
}

int Client::getPort() const
{
	return (_port);
}

int Client::getIndex() const
{
	return (_index);
}

const string& Client::getIP() const
{
	return (_ip);
}

const string& Client::getMessage() const
{
	return (_message);
}