#include "Serv.hpp"

Serv::Serv(ServConf& sc)
{
	const vector<ServBlock>& serv = sc.getServ();

	setAddrInfo();
	setSocket(serv);
	setKqueue();
}
	
Serv::~Serv()
{
	for (unordered_map<int, int>::iterator it = _server.begin(); it != _server.end(); it++)
		close(it->first);
}

void Serv::setAddrInfo()
{
	memset(&_hints, 0, sizeof(_hints));
	_hints.ai_family = AF_INET;
	_hints.ai_socktype = SOCK_STREAM;
	_hints.ai_flags = AI_PASSIVE;
}

int Serv::initSocket(const char* domain, const char* port)
{
	struct addrinfo *res, *p;

	int status = getaddrinfo(domain, port, &_hints, &res);
	if (status != 0)
		throw std::runtime_error("Error: getaddrinfo: " + string(gai_strerror(status)));

	int tmpfd;
	for (p = res; p != nullptr; p = p->ai_next)
	{
		tmpfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (tmpfd == -1) 
			throw std::runtime_error("Error: socket: " + string(strerror(errno)));

		int opt = 1;
		if (setsockopt(tmpfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) == -1)
			throw std::runtime_error("Error: setsockopt: " + string(strerror(errno)));

		if (::bind(tmpfd, p->ai_addr, p->ai_addrlen) == 0)
			break ;

		close(tmpfd);
	}
	freeaddrinfo(res);
	if (p == NULL)
		throw std::runtime_error("Error: Failed to bind with following domain: " + string(domain));

	if (listen(tmpfd, 128) == -1)
		throw std::runtime_error("Error: listen: " + string(strerror(errno)));

	return (tmpfd);
}

void Serv::setSocket(const vector<ServBlock>& serv)
{
	int idx = 0;
	for (vector<ServBlock>::const_iterator it = serv.begin(); it != serv.end(); it++)
	{
		const char* port = it->getPort().c_str();

		if (it->getName().size() == 0)
		{
			for (vector<string>::const_iterator nit = it->getName().begin(); nit != it->getName().end(); nit++)
				_server[initSocket((*nit).c_str(), port)] = idx;
		}
		else
			_server[initSocket(NULL, port)] = idx;
		idx++;
	}
}

void Serv::setKqueue()
{
	_kq = kqueue();
	if (_kq == -1)
		throw std::runtime_error("Error: kqueue: " + string(strerror(errno)));
	_evList.resize(32);

	for (unordered_map<int, int>::iterator it = _server.begin(); it != _server.end(); it++)
	{
		struct kevent evSet;
		EV_SET(&evSet, it->first, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
		if (kevent(_kq, &evSet, 1, NULL, 0, NULL) == -1)
			throw std::runtime_error("Error: kevent: " + string(strerror(errno)));
	}
}

void Serv::acceptClient(int fd)
{
	int client_fd = accept(fd, NULL, NULL);
	if (client_fd == -1)
		throw runtime_error("Error: accept: " + string(strerror(errno)));

	struct kevent evSet;
	EV_SET(&evSet, client_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	if (kevent(_kq, &evSet, 1, NULL, 0, NULL) == -1)
		throw runtime_error("Error: kevent: " + string(strerror(errno)));

	_client[client_fd] = Client(getServerIdx(fd));
	cout << "Client connected\n";
}

void Serv::readClient(int fd, ServConf& conf)
{
	char buffer[1024];
	string totalBuff;
	ssize_t totalSize = 0;

	while (1)
	{
		ssize_t n = read(fd, buffer, sizeof(buffer));
		if (n < 0)
			throw runtime_error("Error: read: ");
		else if (n == 0)
			break ;
		else if (n < 1024)
		{
			buffer[n] = '\0';
			totalBuff += buffer;
			totalSize += n;
			break ;
		}
		else
		{
			totalBuff += buffer;
			totalSize += n;
		}
	}
	struct kevent evSet;
	if (totalSize <= 0)
	{
		EV_SET(&evSet, fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
		if (kevent(_kq, &evSet, 1, NULL, 0, NULL) == -1)
			throw runtime_error("Error: kevent: " + string(strerror(errno)));
		close(fd);
		cout << "Client disconnected\n";
	}
	else
	{
		unordered_map<int, struct Client>::iterator it = _client.find(fd);
		if (it != _client.end())
		{
			// Request request(totalBuff);
			// Responce responce(request, conf, )
			(void) conf;
			it->second._message =
				"HTTP/1.1 200 OK\r\n"
				"Date: Sat, 06 Jan 2025 12:00:00 GMT\r\n"
				"Server: SimpleHTTPServer/1.0\r\n"
				"Content-Type: text/html; charset=UTF-8\r\n"
				"Content-Length: 88\r\n"
				"\r\n"
				"<!DOCTYPE html>\n"
				"<html>\n"
				"<head><title>Hello, World!</title></head>\n"
				"<body><h1>Hello, World!</h1></body>\n"
				"</html>";

			EV_SET(&evSet, fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
			if (kevent(_kq, &evSet, 1, NULL, 0, NULL) == -1)
				throw runtime_error("Error: kevent: " + string(strerror(errno)));
		}
	}
}

void Serv::sendClient(int fd)
{
	unordered_map<int, struct Client>::iterator it = _client.find(fd);
	if (it != _client.end())
	{
		string& message = it->second._message;
		if (write(fd, message.c_str(), message.size()) == -1)
			throw runtime_error("Error: write: ");
		_client.erase(it);

		struct kevent evSet;
		EV_SET(&evSet, fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
		if (kevent(_kq, &evSet, 1, NULL, 0, NULL) == -1)
			throw runtime_error("Error: kevent: " + string(strerror(errno)));
	}
}

int Serv::getKq() const
{
	return (_kq);
}

int Serv::getServerIdx(int fd) const
{
	unordered_map<int, int>::const_iterator it = _server.find(fd);
	if (it != _server.end())
		return (it->second);
	return (-1);
}

int Serv::getClientIdx(int fd) const
{
	unordered_map<int, struct Client>::const_iterator it = _client.find(fd);
	if (it != _client.end())
		return (it->second._index);
	return (-1);
}

vector<struct kevent>& Serv::getEvList()
{
	return (_evList);
}