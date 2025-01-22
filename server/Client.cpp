#include "Client.hpp"

Client::Client() : _port(0), _index(0)
{
	_lastTime = time(NULL);
}

Client::Client(int port, int index, const string& ip) : _port(port), _index(index), _ip(ip)
{
	_lastTime = time(NULL);
}

Client::~Client() {}

void Client::updateTime()
{
	_lastTime = time(NULL);
}

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

time_t Client::getLastTime() const
{
	return (_lastTime);
}

const string& Client::getIP() const
{
	return (_ip);
}

const string& Client::getMessage() const
{
	return (_message);
}
