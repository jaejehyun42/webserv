#ifndef SERVER_HPP
# define SERVER_HPP

#include <vector>
#include <unordered_map>
#include <cstring>
#include <iostream>
#include <time.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "Client.hpp"
#include "../config/ServConf.hpp"
#include "../request/Request.hpp"

class Server
{
private:
	int _kq;
	struct timespec _timeout;
	vector<struct kevent> _evList;

	struct addrinfo _hints;
	unordered_map<int, int> _server;
	unordered_map<int, Client> _client;

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
	void closeClient(int fd);
	void checkTimeout(long timeout);

	int getServerIdx(int fd) const;
	int getClientIdx(int fd) const;

	int getKq() const;
	int getKevent();
	const struct kevent& getEvList(int idx) const;
};

#endif