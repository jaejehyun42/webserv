#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <ctime>
#include <string>

using namespace std;

class Client
{
private:
	int _port;
	int _index;
	size_t _size;
	string _ip;
	string _message;
	time_t _lastTime;
public:
	Client();
	Client(int port, int index, const string& ip);
	~Client();

	void updateTime();
	void setMessage(const string& message, size_t size);

	int getPort() const;
	int getIndex() const;
	size_t getSize() const;
	time_t getLastTime() const;
	const string& getIP() const;
	const string& getMessage() const;
};

#endif
