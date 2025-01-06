#ifndef CONFIG_HPP
# define CONFIG_HPP

#include <map>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <exception>

#define GET 0
#define POST 1
#define DELETE 2

typedef struct s_location
{
	bool method[3]; // 인덱스 순서는 GET, POST, DELETE 순서
	bool autoindex;
	std::string root;
	std::string cgiPass;

	std::vector<std::string> index;
} t_location;

typedef struct s_serv
{
	int port;
	int maxSize;
	std::string root;
	std::string name;

	std::map<long, std::string> error;
	std::map<std::string, struct s_location> path;
} t_serv;

class ServConf
{
private:
	long _aliveTime;
	std::map<std::string, std::string> _mime;
	std::vector<struct s_serv> _serv;

	void _includeFile(const std::string& fileName);

	void _parseMime(std::ifstream& file);
	void _parseHTTP(std::ifstream& file, bool inc);
	struct s_serv _parseServ(std::ifstream& file);
	struct s_location _parsePath(std::ifstream& file, const std::string& path);
public:
	ServConf();
	ServConf(const ServConf& ref);
	ServConf& operator=(const ServConf& ref);
	~ServConf();

	void parse(const std::string& fileName);

	void print() const; // 제대로 넣어졌는지 체크 용도
};

#endif