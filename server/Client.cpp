#include "Client.hpp"

Client::Client() : _port(0), _index(0)
{
	_lastTime = time(NULL);
	_size = 0;
}

Client::Client(int port, int index, const string& ip) : _port(port), _index(index), _ip(ip)
{
	_lastTime = time(NULL);
	_size = 0;
}

Client::~Client()
{
	_message.clear();
}

void Client::updateTime()
{
	_lastTime = time(NULL);
}

void Client::setMessage(const string& message, size_t size)
{
	if (message == "")
	{
		_message.clear();
		_message = "";
		_size = 0;
	}
	else
	{
		_message.append(message);
		_size += size;
	}
}

int Client::getPort() const
{
	return (_port);
}

int Client::getIndex() const
{
	return (_index);
}

size_t Client::getSize() const
{
	return (_size);
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
