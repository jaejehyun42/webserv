#ifndef CONFIG_HPP
# define CONFIG_HPP

#include <map>
#include <vector>
#include <fstream>
#include <iostream>
#include <exception>

typedef struct location
{
	bool autoindex;
	std::string index;
	std::string cgiPass;
} location;

typedef struct server
{
	int port;
	int maxSize;
	std::string name;
	std::string root;

	std::map<std::string, std::string> error;
	std::map<std::string, struct location> path;
} server;

class ServConf
{
private:
	int aliveTime;
	std::map<std::string, std::string> mime;
	std::vector<struct server> serv;
public:
	ServConf();
	ServConf(const ServConf& ref);
	ServConf& operator=(const ServConf& ref);
	~ServConf();

	void parse(const std::string& fileName);
	void setConf();

	int getAliveTime() const;
	const std::string& getContentType(const std::string& ext) const;

	int getServSize() const;
	int getServPort(int idx) const;

	const std::string& getErrorPage(int status) const;
	const std::string& getPath(const std::string& path);
};

#endif