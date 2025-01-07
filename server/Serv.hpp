#ifndef SERV_HPP
# define SERV_HPP

#include <cstring>
#include <iostream>
#include <netdb.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <sys/socket.h>
#include "../config/ServConf.hpp"

class Serv
{
private:
	int _kq;
	struct addrinfo _hints;
	vector<int> _sockfd;
	vector<struct kevent> _evList;
	

	void setAddrInfo();
	void setSocket(const vector<ServBlock>& serv);
	void setKqueue();
public:
	Serv(ServConf& servConf);
	~Serv();

	bool checkSockfd(int fd);

	int getKq() const;
	vector<int>& getSockfd();
	vector<struct kevent>& getEvList();
};

#endif