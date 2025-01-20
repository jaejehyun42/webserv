#include "Server.hpp"

Server::Server(ServConf& sc)
{
	const vector<ServBlock>& serv = sc.getServ();

	_setAddrInfo();
	_setSocket(serv);
	_setKqueue();

	_timeout.tv_sec = 1;
	_timeout.tv_nsec = 0;
}
	
Server::~Server()
{
	for (unordered_map<int, int>::iterator it = _server.begin(); it != _server.end(); it++)
		close(it->first);
}

void Server::_setAddrInfo()
{
	memset(&_hints, 0, sizeof(_hints));
	_hints.ai_family = AF_INET;
	_hints.ai_socktype = SOCK_STREAM;
	_hints.ai_flags = AI_PASSIVE;
}

int Server::_initSocket(const char* domain, const char* port)
{
	struct addrinfo *res, *p;

	int status = getaddrinfo(domain, port, &_hints, &res);
	if (status != 0)
		throw runtime_error("Error: getaddrinfo: " + string(gai_strerror(status)));

	int tmpfd;
	for (p = res; p != NULL; p = p->ai_next)
	{
		tmpfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (tmpfd == -1) 
			throw runtime_error("Error: socket: " + string(strerror(errno)));

		int opt = 1;
		if (setsockopt(tmpfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) == -1)
			throw runtime_error("Error: setsockopt: " + string(strerror(errno)));

		if (::bind(tmpfd, p->ai_addr, p->ai_addrlen) == 0)
			break ;

		close(tmpfd);
	}
	freeaddrinfo(res);
	if (p == NULL)
		throw runtime_error("Error: Failed to bind with following domain: " + string(domain));

	if (listen(tmpfd, MAX_EVENTS) == -1)
		throw runtime_error("Error: listen: " + string(strerror(errno)));

	return (tmpfd);
}

void Server::_setSocket(const vector<ServBlock>& serv)
{
	int idx = 0;
	for (vector<ServBlock>::const_iterator it = serv.begin(); it != serv.end(); it++)
	{
		const char* port = it->getPort().c_str();

		if (it->getName().size() != 0)
		{
			for (vector<string>::const_iterator nit = it->getName().begin(); nit != it->getName().end(); nit++)
				_server[_initSocket((*nit).c_str(), port)] = idx;
		}
		else
			_server[_initSocket(NULL, port)] = idx;
		idx++;
	}
}

void Server::_setKqueue()
{
	_kq = kqueue();
	if (_kq == -1)
		throw runtime_error("Error: kqueue: " + string(strerror(errno)));
	_evList.resize(MAX_EVENTS);

	for (unordered_map<int, int>::iterator it = _server.begin(); it != _server.end(); it++)
	{
		struct kevent evSet;
		EV_SET(&evSet, it->first, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
		if (kevent(_kq, &evSet, 1, NULL, 0, NULL) == -1)
			throw runtime_error("Error: kevent: " + string(strerror(errno)));
	}
}

void Server::_setEvent(int fd, int filter, int flags)
{
	struct kevent evSet;
	EV_SET(&evSet, fd, filter, flags, 0, 0, NULL);
	if (kevent(_kq, &evSet, 1, NULL, 0, NULL) == -1)
		throw runtime_error("Error: kevent: " + string(strerror(errno)));
	_client[fd].updateTime();
}

void Server::_sendError(int fd)
{
	string error = "<!DOCTYPE html>\n"
					"<head>\n"
					"	<title>Error</title>\n"
					"</head>\n"
					"<body>\n"
					"	<h1>400</h1>\n"
					"	<p>400 Bad Request</p>\n"
					"</body>\n"
					"</html>";

	string response = "HTTP/1.1 404 Not Found\r\n"
					"Content-Type: text/html\r\n"
					"Content-Length: " + std::to_string(error.size()) + "\r\n"
					"\r\n" +
					error;

	if (write(fd, response.c_str(), response.size()) <= 0)
		closeClient(fd);
}

void Server::acceptClient(int fd)
{
	struct sockaddr_in addr;
	socklen_t addr_len = sizeof(addr);

	int client_fd = accept(fd, (struct sockaddr*)&addr, &addr_len);
	if (client_fd == -1)
		throw runtime_error("Error: accept: " + string(strerror(errno)));
	if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1)
		throw runtime_error("Error: fcntl(F_SETFL, O_NONBLOCK)");

	char ip[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &addr.sin_addr, ip, sizeof(ip));
	int port = ntohs(addr.sin_port);

	_client[client_fd] = Client(port, getServerIdx(fd), ip);
	_setEvent(client_fd, EVFILT_READ, EV_ADD | EV_ENABLE);
}

void Server::readClient(int fd, const ServConf& conf)
{
	char buffer[1024] = {0};

	ssize_t size = read(fd, buffer, sizeof(buffer));
	if (size <= 0)
	{
		closeClient(fd);
		return ;
	}
	_client[fd].setMessage(buffer);
	
	const string& message = _client[fd].getMessage();
	size_t headerEnd = message.find("\r\n\r\n");
	size_t maxSize = conf.getServBlock(_client[fd].getIndex()).getMaxSize();
	if (headerEnd != string::npos)
	{
		headerEnd += 4;
		if (headerEnd > 8192)
			return _sendError(fd);
		string header = message.substr(0, headerEnd);
		size_t lenPos = header.find("Content-Length:");
		size_t chunkPos = header.find("Transfer-Encoding: chunked");

		if (lenPos != string::npos)
		{
			size_t start = lenPos + 16;
			size_t end = header.find("\r\n", start);
			if (end == string::npos)
				return _sendError(fd);

			size_t bodySize = message.size() - headerEnd;
			long contentLength = strtol(header.substr(start, end - start).c_str(), NULL, 10);

			if (static_cast<size_t>(contentLength) > maxSize)
				return _sendError(fd);
			if (bodySize >= static_cast<size_t>(contentLength))
				_setEvent(fd, EVFILT_WRITE, EV_ADD | EV_ENABLE);
		}
		else if (chunkPos != string::npos)
		{
			string body = message.substr(headerEnd);
			size_t pos = 0;

			while (pos < body.size())
			{
				size_t end = body.find("\r\n", pos);
				if (end == string::npos)
					break ;

				long chunkSize = strtol(body.substr(pos, end - pos).c_str(), NULL, 16);
				pos = end;

				if (chunkSize == 0 && body.substr(pos, 4) == "\r\n\r\n")
					_setEvent(fd, EVFILT_WRITE, EV_ADD | EV_ENABLE);

				pos += chunkSize + 2;
				if (pos > maxSize)
					return _sendError(fd);
			}
		}
		else
			_setEvent(fd, EVFILT_WRITE, EV_ADD | EV_ENABLE);
	}
}

void Server::sendClient(int fd, const ServConf& conf)
{
	unordered_map<int, Client>::iterator it = _client.find(fd);
	if (it != _client.end() && it->second.getMessage() != "")
	{
		Request req;
		req.initRequest(it->second.getMessage());
		Response res(req, conf, it->second.getIndex());

		printLog(fd, req);
		if (write(fd, res.getMessage().c_str(), res.getMessage().size()) <= 0)
			closeClient(fd);

		_setEvent(fd, EVFILT_WRITE, EV_DELETE);
		if (req.chkConnection())
			closeClient(fd);
		else
			it->second.setMessage("");
	}
}

void Server::closeClient(int fd)
{
	close(fd);
	_client.erase(fd);
}

void Server::printLog(int fd, Request& req)
{
	time_t now = time(NULL);
	char* timeInfo = ctime(&now);
	unordered_map<int, Client>::iterator it = _client.find(fd);

	timeInfo[strlen(timeInfo) - 1] = '\0';
	cout << BLUE << "\r[" << timeInfo << "] " << RESET;
	cout << it->second.getIP() << ":" << it->second.getPort() << " > ";
	cout << req.getMethod() << " " << req.getUrl() << RESET << endl;
}

void Server::checkTimeout(long timeout)
{
	if (timeout == 0)
		return ;
		
	time_t now = time(NULL); 
	for (unordered_map<int, Client>::iterator it = _client.begin(); it != _client.end(); it++)
	{
		time_t last = it->second.getLastTime();
		if (now - last > timeout)
			closeClient(it->first);
	}
}

// getter 함수
int Server::getServerIdx(int fd) const
{
	unordered_map<int, int>::const_iterator it = _server.find(fd);
	if (it != _server.end())
		return (it->second);
	return (-1);
}

int Server::getClientIdx(int fd) const
{
	unordered_map<int, Client>::const_iterator it = _client.find(fd);
	if (it != _client.end())
		return (it->second.getIndex());
	return (-1);
}

int Server::getKq() const
{
	return (_kq);
}

int Server::getKevent()
{
	int nev = kevent(_kq, NULL, 0, _evList.data(), MAX_EVENTS, &_timeout);
	if (nev == -1)
		throw runtime_error("kevent: " + string(strerror(errno)));
	return (nev);
}

const struct kevent& Server::getEvList(int idx) const
{
	return (_evList[idx]);
}