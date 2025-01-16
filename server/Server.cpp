#include "Server.hpp"

Server::Server(ServConf& sc)
{
	const vector<ServBlock>& serv = sc.getServ();

	setAddrInfo();
	setSocket(serv);
	setKqueue();

	_timeout.tv_sec = 5;
	_timeout.tv_nsec = 0;
}
	
Server::~Server()
{
	for (unordered_map<int, int>::iterator it = _server.begin(); it != _server.end(); it++)
		close(it->first);
}

void Server::setAddrInfo()
{
	memset(&_hints, 0, sizeof(_hints));
	_hints.ai_family = AF_INET;
	_hints.ai_socktype = SOCK_STREAM;
	_hints.ai_flags = AI_PASSIVE;
}

int Server::initSocket(const char* domain, const char* port)
{
	struct addrinfo *res, *p;

	int status = getaddrinfo(domain, port, &_hints, &res);
	if (status != 0)
		throw std::runtime_error("Error: getaddrinfo: " + string(gai_strerror(status)));

	int tmpfd;
	for (p = res; p != NULL; p = p->ai_next)
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

void Server::setSocket(const vector<ServBlock>& serv)
{
	int idx = 0;
	for (vector<ServBlock>::const_iterator it = serv.begin(); it != serv.end(); it++)
	{
		const char* port = it->getPort().c_str();

		if (it->getName().size() != 0)
		{
			for (vector<string>::const_iterator nit = it->getName().begin(); nit != it->getName().end(); nit++)
				_server[initSocket((*nit).c_str(), port)] = idx;
		}
		else
			_server[initSocket(NULL, port)] = idx;
		idx++;
	}
}

void Server::setKqueue()
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

void Server::setNonBlock(int fd)
{
	if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1) {
		throw runtime_error("Error: fcntl(F_SETFL, O_NONBLOCK)");
	}
}

void Server::acceptClient(int fd)
{
	struct sockaddr_in addr;
	socklen_t addr_len = sizeof(addr);

	int client_fd = accept(fd, (struct sockaddr*)&addr, &addr_len);
	if (client_fd == -1)
		throw runtime_error("Error: accept: " + string(strerror(errno)));

	setNonBlock(client_fd);

	struct kevent evSet;
	EV_SET(&evSet, client_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	if (kevent(_kq, &evSet, 1, NULL, 0, NULL) == -1)
		throw runtime_error("Error: kevent: " + string(strerror(errno)));

	char ip[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &addr.sin_addr, ip, sizeof(ip));
	int port = ntohs(addr.sin_port);

	_client[client_fd] = Client(port, getServerIdx(fd), ip);
}

void Server::readClient(int fd)
{
    char buffer[1024] = {0};

    ssize_t size = read(fd, buffer, sizeof(buffer));
    if (size < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return;
        }
        closeClient(fd);
        return;
    }
    if (size == 0) {
        closeClient(fd);
        return;
    }
	_client[fd].setMessage(buffer);
	
	const std::string& message = _client[fd].getMessage();
    size_t headerEnd = message.find("\r\n\r\n");
    if (headerEnd != std::string::npos)
	{
        // 헤더 파싱
        std::string header = message.substr(0, headerEnd + 4);

        // 2. Content-Length 확인
        size_t contentLengthPos = header.find("Content-Length:");
        int contentLength = 0;
        if (contentLengthPos != std::string::npos) {
            size_t start = contentLengthPos + 15;
            size_t end = header.find("\r\n", start);
            contentLength = std::stoi(header.substr(start, end - start));
        }

        // 3. 바디의 시작 위치와 현재 크기 확인
        size_t bodyStart = headerEnd + 4;
        size_t bodySize = message.size() - bodyStart;

        // 4. 바디가 완전히 수신되었는지 확인
        if (bodySize >= static_cast<size_t>(contentLength)) {
			struct kevent evSet;
			EV_SET(&evSet, fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
			if (kevent(_kq, &evSet, 1, NULL, 0, NULL) == -1)
				throw runtime_error("Error: kevent: " + string(strerror(errno)));
        }
    }
}

void Server::sendClient(int fd, const ServConf& conf)
{
	unordered_map<int, Client>::iterator it = _client.find(fd);
	if (it != _client.end() && it->second.getMessage() != "")
	{
		Request req;
		try
		{
			req.initRequest(it->second.getMessage());
		}
		catch(const std::exception& e)
		{
			std::cerr << "\rreq: " << typeid(e).name() << " - " << e.what() << std::endl;
		}
		Response res(req, conf, it->second.getIndex());
		printLog(fd, req);
		if (write(fd, res.getMessage().c_str(), res.getMessage().size()) == -1)
			throw runtime_error("Error: write: ");

		struct kevent evSet;
		EV_SET(&evSet, fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
		if (kevent(_kq, &evSet, 1, NULL, 0, NULL) == -1)
			throw runtime_error("Error: kevent: " + string(strerror(errno)));

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
		if (now - it->second.getLastTime() > timeout)
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
	int nev = kevent(_kq, NULL, 0, _evList.data(), 32, &_timeout);
	if (nev == -1)
		throw runtime_error("kevent: " + string(strerror(errno)));
	return (nev);
}

const struct kevent& Server::getEvList(int idx) const
{
	return (_evList[idx]);
}