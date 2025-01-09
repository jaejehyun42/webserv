#include "Server.hpp"

Server::Server(ServConf& sc)
{
	const vector<ServBlock>& serv = sc.getServ();

	setAddrInfo();
	setSocket(serv);
	setKqueue();
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

void Server::acceptClient(int fd)
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

void Server::readClient(int fd)
{
	char buffer[1024] = {0};
	ssize_t size = read(fd, buffer, sizeof(buffer));
	if (size <= 0)
	{
		close(fd);
		cout << "Client disconnected\n";
		return ;
	}
	_client[fd]._message += buffer;
	
	struct kevent evSet;
	EV_SET(&evSet, fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
	if (kevent(_kq, &evSet, 1, NULL, 0, NULL) == -1)
		throw runtime_error("Error: kevent: " + string(strerror(errno)));
}

void Server::sendClient(int fd)
{
	unordered_map<int, struct Client>::iterator it = _client.find(fd);
	if (it != _client.end())
	{
		Request request;
		request.initRequest(it->second._message);

		// cout << "Method = " << request.getMethod() << "\n";
		// cout << "URL = " << request.getUrl() << "\n";
		// cout << "Path = " << request.getPath() << "\n";
		// cout << "Query = " << request.getQuery() << "\n";
		// cout << "CgiPath = " << request.getCgiPath() << "\n";
		// cout << "Version = " << request.getVersion() << "\n";
		// unordered_map<string, string> headers = request.getHeaders();
		// cout << "Headers = " << "\n";
		// for (Request::umap_it it = headers.begin(); it != headers.end(); ++it)
		// 	cout << it->first << ": " << it->second << "\n";
		// cout << "Body = " << "\n" << request.getBody() << endl;

		string message =
			"HTTP/1.1 200 OK\r\n"
			"Date: Sat, 06 Jan 2025 12:00:00 GMT\r\n"
			"Server: SimpleHTTPServer/1.0\r\n"
			"Content-Type: text/html; charset=UTF-8\r\n"
			"Content-Length: 88\r\n"
			"Connection: Keep-Alive\r\n"
			"\r\n"
			"<!DOCTYPE html>\n"
			"<html>\n"
			"<head><title>Hello, World!</title></head>\n"
			"<body><h1>Hello, World!</h1></body>\n"
			"</html>";

		if (write(fd, message.c_str(), message.size()) == -1)
			throw runtime_error("Error: write: ");

		struct kevent evSet;
		EV_SET(&evSet, fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
		if (kevent(_kq, &evSet, 1, NULL, 0, NULL) == -1)
			throw runtime_error("Error: kevent: " + string(strerror(errno)));
		_client.erase(it);
	}
}

int Server::getServerIdx(int fd) const
{
	unordered_map<int, int>::const_iterator it = _server.find(fd);
	if (it != _server.end())
		return (it->second);
	return (-1);
}

int Server::getClientIdx(int fd) const
{
	unordered_map<int, struct Client>::const_iterator it = _client.find(fd);
	if (it != _client.end())
		return (it->second._index);
	return (-1);
}

int Server::getKq() const
{
	return (_kq);
}

int Server::getKevent()
{
	int nev = kevent(_kq, NULL, 0, _evList.data(), 32, NULL);
	return (nev);
}

vector<struct kevent>& Server::getEvList()
{
	return (_evList);
}