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
#include "../request/Request.hpp"

struct Client
{
	int _index;
	string _message;

	Client() : _index(0), _message("") {}
	Client(int n) : _index(n), _message("") {}
};

class Server
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
	Server(ServConf& servConf);
	~Server();

	void acceptClient(int fd);
	void readClient(int fd);
	void sendClient(int fd);

	int getServerIdx(int fd) const;
	int getClientIdx(int fd) const;

	int getKq() const;
	int getKevent();
	vector<struct kevent>& getEvList();
};

#endif