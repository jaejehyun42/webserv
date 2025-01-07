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
	for (vector<int>::iterator it = _sockfd.begin(); it != _sockfd.end(); it++)
		close(*it);
}

void Serv::setAddrInfo()
{
	memset(&_hints, 0, sizeof(_hints));
	_hints.ai_family = AF_INET;
	_hints.ai_socktype = SOCK_STREAM;
	_hints.ai_flags = AI_PASSIVE;
}

void Serv::setSocket(const vector<ServBlock>& serv)
{
	for (vector<ServBlock>::const_iterator it = serv.begin(); it != serv.end(); it++)
	{
		struct addrinfo *res, *p;

		int status = getaddrinfo(it->getName().c_str(), it->getPort().c_str(), &_hints, &res);
		if (status != 0)
			throw std::runtime_error("getaddrinfo: " + string(gai_strerror(status)));

		int tmpfd;
		for (p = res; p != nullptr; p = p->ai_next)
		{
			tmpfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
			if (tmpfd == -1) 
				throw std::runtime_error("socket: " + string(strerror(errno)));

			int opt = 1;
			if (setsockopt(tmpfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) == -1)
				throw std::runtime_error("setsockopt: " + string(strerror(errno)));

			if (::bind(tmpfd, p->ai_addr, p->ai_addrlen) == 0)
				break ;

			close(tmpfd);
		}

		freeaddrinfo(res);
		if (p == NULL)
			throw std::runtime_error("Failed to bind to any address.");

		if (listen(tmpfd, 10000) == -1)
			throw std::runtime_error("listen: " + string(strerror(errno)));

		struct sockaddr_storage local_addr;
		socklen_t local_addr_len = sizeof(local_addr);
		if (getsockname(tmpfd, (struct sockaddr*)&local_addr, &local_addr_len) == 0) {
			char host[NI_MAXHOST];
			if (getnameinfo((struct sockaddr*)&local_addr, local_addr_len, host, sizeof(host), NULL, 0, NI_NUMERICHOST) == 0) {
				std::cout << "Server is running on IP: " << host << ", Port: " << it->getPort() << std::endl;
			}
		}

		_sockfd.push_back(tmpfd);
	}
}

void Serv::setKqueue()
{
	_kq = kqueue();
	if (_kq == -1)
		throw std::runtime_error("kqueue: " + string(strerror(errno)));
	_evList.resize(32);

	for (vector<int>::iterator it = _sockfd.begin(); it != _sockfd.end(); it++)
	{
		struct kevent evSet;
		EV_SET(&evSet, *it, EVFILT_READ, EV_ADD, 0, 0, NULL);
		if (kevent(_kq, &evSet, 1, NULL, 0, NULL) == -1)
			throw std::runtime_error("kevent: " + string(strerror(errno)));
	}
}

bool Serv::checkSockfd(int fd)
{
	for (vector<int>::iterator it = _sockfd.begin(); it != _sockfd.end(); it++)
	{
		cout << "fd: " << *it << endl;
		if (*it == fd)
			return (true);
	}
	return (false);
}

int Serv::getKq() const
{
	return (_kq);
}

vector<int>& Serv::getSockfd()
{
	return (_sockfd);
}

vector<struct kevent>& Serv::getEvList()
{
	return (_evList);
}