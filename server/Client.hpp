#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <string>

using namespace std;

class Client
{
private:
	int _port;
	int _index;
	string _ip;
	string _message;
public:
	Client();
	Client(int port, int index, const string& ip);
	~Client();

	void init(int index, int port, const string& ip);
	void setMessage(const string& message);

	int getPort() const;
	int getIndex() const;
	const string& getIP() const;
	const string& getMessage() const;
};

#endif