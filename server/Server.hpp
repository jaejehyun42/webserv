#ifndef SERVER_HPP
# define SERVER_HPP

#include <vector>
#include <unordered_map>
#include <cstring>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <time.h>
#include <fcntl.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "Client.hpp"
#include "../config/ServConf.hpp"
#include "../request/Request.hpp"
#include "../response/Response.hpp"

#define RESET	"\033[0m"
#define RED		"\033[31m"
#define GREEN	"\033[32m"
#define YELLOW	"\033[33m"
#define BLUE	"\033[34m"

#define MAX_EVENTS 128

class Server
{
private:
	int _kq;
	struct timespec _timeout;
	vector<struct kevent> _evList;

	struct addrinfo _hints;
	unordered_map<int, int> _server;
	unordered_map<int, Client> _client;

	void _setAddrInfo();
	int _initSocket(const char* domain, const char* port);
	void _setSocket(const vector<ServBlock>& serv);
	void _setKqueue();
	void _setEvent(int fd, int filter, int flags);
	void _sendError(int fd, const string& status, const string& phrase, const ServConf& conf);
	void _printLog(int fd, const string& one, const string& two, const string& color);
public:
	Server(ServConf& servConf);
	~Server();

	void acceptClient(int fd);
	void readClient(int fd, const ServConf& conf);
	void sendClient(int fd, const ServConf& conf);
	void closeClient(int fd);

	void checkTimeout(long timeout);

	int getServerIdx(int fd) const;
	int getClientIdx(int fd) const;

	int getKq() const;
	int getKevent();
	const struct kevent& getEvList(int idx) const;
};

#endif