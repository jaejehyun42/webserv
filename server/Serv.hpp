#ifndef SERV_HPP
# define SERV_HPP

#include <vector>
#include <unordered_map>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <sys/socket.h>

#include "../config/ServConf.hpp"

struct Client
{
	int _index;
	string _message;

	Client() : _index(0), _message("") {}
	Client(int n) : _index(n), _message("") {}
};

class Serv
{
private:
	int _kq;
	struct addrinfo _hints;
	vector<struct kevent> _evList;
	unordered_map<int, int> _server;
	unordered_map<int, struct Client> _client;

	void setAddrInfo();
	int initSocket(const char* domain, const char* port);
	void setSocket(const vector<ServBlock>& serv);
	void setKqueue();
public:
	Serv(ServConf& servConf);
	~Serv();

	void acceptClient(int fd);
	void readClient(int fd, ServConf& sc);
	void sendClient(int fd);

	int getKq() const;
	int getServerIdx(int fd) const;
	int getClientIdx(int fd) const;
	vector<struct kevent>& getEvList();
};

#endif