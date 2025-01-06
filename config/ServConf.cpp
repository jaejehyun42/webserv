#include "ServConf.hpp"

ServConf::ServConf() {}

ServConf::ServConf(const ServConf& ref)
{
	*this = ref;
}

ServConf& ServConf::operator=(const ServConf& ref)
{
	if (this != &ref)
	{
		_aliveTime = ref._aliveTime;
		_mime = ref._mime;
		_serv = ref._serv;
	}
	return (*this);
}

ServConf::~ServConf() {}

std::vector<std::string> splitString(std::string& line)
{
	std::string	token;
	std::istringstream iss(line);
	std::vector<std::string> result;

	while (iss >> token)
		result.push_back(token);

	return (result);
}

void ServConf::_includeFile(const std::string& fileName)
{
	std::ifstream file(fileName.c_str());
	if (!file)
		throw std::runtime_error("Error: 파일이 없거나 열 수 없습니다.");

	_parseHTTP(file, true);
}

void ServConf::_parseMime(std::ifstream& file)
{
	std::string line;
	
	while (std::getline(file, line))
	{
		if (line == "")
			continue ;

		std::vector<std::string> tokens = splitString(line);
		if (tokens.size() == 0)
			continue ;
		if (tokens.front().front() == '#')
			continue ;
		if (tokens.size() == 1)
		{
			if (tokens.back() == "}")
				break ;
			else
				throw std::runtime_error("Error: http 블록 포멧이 잘못 되었습니다");
		}
		if (tokens.back().back() != ';')
			throw std::runtime_error("Error: http 블록 포멧이 잘못 되었습니다");

		std::string& value = tokens.front();

		for (std::vector<std::string>::iterator it = tokens.begin() + 1; it != tokens.end(); it++)
		{
			std::string& key = *it;
			if (key.back() == ';')
				key.pop_back();
			_mime[key] = value;
		}
	}
	if (line.find("}") == std::string::npos)
		throw std::runtime_error("Error: http 블록 포멧이 잘못 되었습니다.");
}

struct s_location ServConf::_parsePath(std::ifstream& file, const std::string& path)
{
	std::string line;
	struct s_location temp;
	temp.method[GET] = true;
	temp.method[POST] = true;
	temp.method[DELETE] = true;

	while (std::getline(file, line))
	{
		if (line == "")
			continue ;

		std::vector<std::string> tokens = splitString(line);
		if (tokens.size() == 0)
			continue ;
		if (tokens.front().front() == '#')
			continue ;
		if (tokens.size() == 1)
		{
			if (tokens.back() == "}")
				break ;
			else
				throw std::runtime_error("Error: location 블록 포멧이 잘못 되었습니다");
		}

		std::string& key = tokens.front();
		std::string& value = tokens.back();

		if (value.back() == ';')
		{
			value.pop_back();
			if (key == "index")
			{
				for (std::vector<std::string>::iterator it = tokens.begin() + 1; it != tokens.end(); it++)
					temp.index.push_back(*it);
			}
			else if (key == "cgi_pass")
				temp.cgiPass = value;
			else if (key == "root")
				temp.root = value;
			else if (key == "autoindex")
			{
				if (value == "on")
					temp.autoindex = true;
				else
					temp.autoindex = false;
			}
			else if (key == "limit_except")
			{
				int check[3] = {0};
				for (std::vector<std::string>::iterator it = tokens.begin() + 1; it != tokens.end(); it++)
				{
					if (*it == "GET")
						check[GET] = 1;
					else if (*it == "POST")
						check[POST] = 1;
					else if (*it == "DELETE")
						check[DELETE] = 1;
					else
						throw std::runtime_error("Error: 지원하는 서버 옵션이 아닙니다.");
				}
				if (check[GET] == 0)
					throw std::runtime_error("Error: 지원하는 서버 옵션이 아닙니다.");
				if (check[POST] == 0)
					throw std::runtime_error("Error: 지원하는 서버 옵션이 아닙니다.");
				if (check[DELETE] == 0)
					temp.method[DELETE] = 0;
			}
			else
				throw std::runtime_error("Error: 지원하는 서버 옵션이 아닙니다.");
		}
		else
			throw std::runtime_error("Error: location 블록 포멧이 잘못 되었습니다");
	}
	if (line.find("}") == std::string::npos)
		throw std::runtime_error("Error: location 블록 포멧이 잘못 되었습니다.");
	if (path.front() == '.' && temp.index.size() > 0)
		throw std::runtime_error("Error: location 블록 요소가 잘못 되었습니다.");
	if (path.front() == '/' && temp.cgiPass != "")
		throw std::runtime_error("Error: location 블록 요소가 잘못 되었습니다.");
	return (temp);
}

struct s_serv ServConf::_parseServ(std::ifstream& file)
{
	std::string line;
	struct s_serv temp;
	temp.port = 0;
	temp.maxSize = 0;
	
	while (std::getline(file, line))
	{
		if (line == "")
			continue ;

		std::vector<std::string> tokens = splitString(line);
		if (tokens.size() == 0)
			continue ;
		if (tokens.front().front() == '#')
			continue ;
		if (tokens.size() == 1)
		{
			if (tokens.back() == "}")
				break ;
			else
				throw std::runtime_error("Error: server 블록 포멧이 잘못 되었습니다");
		}

		std::string& key = tokens.front();
		std::string& value = tokens.back();

		if (value == "{")
		{
			if (key == "location")
			{
				if (tokens.size() == 2)
					throw std::runtime_error("Error: location 블록 포멧이 잘못 되었습니다");

				std::string& path = *(tokens.begin() + 1);
				temp.path[path] = _parsePath(file, path);
			}
			else
				throw std::runtime_error("Error: 지원하는 서버 옵션이 아닙니다.");
		}
		else if (value.back() == ';')
		{
			value.pop_back();
			if (key == "listen")
				temp.port = strtol(value.c_str(), NULL, 10);
			else if (key == "client_max_body_size")
				temp.maxSize = strtol(value.c_str(), NULL, 10);
			else if (key == "root")
				temp.root = value;
			else if (key == "server_name")
				temp.name = value;
			else if (key == "error_page")
			{
				if (tokens.size() <= 2)
					throw std::runtime_error("Error: server 블록 포멧이 잘못 되었습니다");
				for (std::vector<std::string>::iterator it = tokens.begin() + 1; it != tokens.end() - 1; it++)
					temp.error[strtol((*it).c_str(), NULL, 10)] = value;
			}
			else
				throw std::runtime_error("Error: 지원하는 서버 옵션이 아닙니다.");
		}
		else
			throw std::runtime_error("Error: server 블록 포멧이 잘못 되었습니다");
	}
	if (line.find("}") == std::string::npos)
		throw std::runtime_error("Error: server 블록 포멧이 잘못 되었습니다.");
	if (temp.port == 0 || temp.maxSize == 0)
		throw std::runtime_error("Error: 설정 파일의 필수 구성 요소가 없습니다.");
	if (temp.error.size() == 0 || temp.path.size() == 0)
		throw std::runtime_error("Error: 설정 파일의 필수 구성 요소가 없습니다.");
	return (temp);
}

void ServConf::_parseHTTP(std::ifstream& file, bool inc)
{
	std::string line;
	while (std::getline(file, line))
	{
		if (line == "")
			continue ;

		std::vector<std::string> tokens = splitString(line);
		if (tokens.size() == 0)
			continue ;
		if (tokens.front().front() == '#')
			continue ;
		if (tokens.size() == 1)
		{
			if (tokens.back() == "}")
				break ;
			else
				throw std::runtime_error("Error: http 블록 포멧이 잘못 되었습니다");
		}

		std::string& key = tokens.front();
		std::string& value = tokens.back();

		if (value == "{")
		{
			if (key == "server")
				_serv.push_back(_parseServ(file));
			else if (key == "types")
				_parseMime(file);
			else
				throw std::runtime_error("Error: 지원하는 서버 옵션이 아닙니다.");
		}
		else if (value.back() == ';')
		{
			value.pop_back();
			if (key == "include")
				_includeFile(value);
			else if (key == "default_type")
				_mime["default"] = value;
			else if (key == "keepalive_timeout")
				_aliveTime = strtol(value.c_str(), NULL, 10);
			else
				throw std::runtime_error("Error: 지원하는 서버 옵션이 아닙니다.");
		}
		else
			throw std::runtime_error("Error: http 블록 포멧이 잘못 되었습니다.");
	}
	if (!inc && line.find("}") == std::string::npos)
		throw std::runtime_error("Error: http 블록 포멧이 잘못 되었습니다.");
}

void ServConf::parse(const std::string& fileName)
{
	std::ifstream file(fileName.c_str());
	if (!file)
		throw std::runtime_error("Error: 파일이 없거나 열 수 없습니다.");

	std::string line;
	while (std::getline(file, line))
	{
		if (line == "")
			continue ;

		std::vector<std::string> tokens = splitString(line);
		if (tokens.size() == 0)
			continue ;
		if (tokens.front().front() == '#')
			continue ;
		if (tokens.size() == 1)
			throw std::runtime_error("Error: 파일 포멧이 잘못 되었습니다");

		std::string& key = tokens.front();
		std::string& value = tokens.back();

		if (value == "{")
		{
			if (key == "http")
				_parseHTTP(file, false);
			else
				throw std::runtime_error("Error: 지원하는 서버 옵션이 아닙니다.");
		}
		else if (value.back() == ';')
			throw std::runtime_error("Error: 지원하는 서버 옵션이 아닙니다.");
		else
			throw std::runtime_error("Error: 파일 포멧이 잘못 되었습니다.");
	}
	if (_serv.size() < 1 || _mime.find("default") == _mime.end())
		throw std::runtime_error("Error: 설정 파일의 필수 구성 요소가 없습니다.");
}

void ServConf::print() const
{
	std::cout << "keepalive_timeout: " << _aliveTime << std::endl;

	std::cout << "type {\n";
	for (std::map<std::string, std::string>::const_iterator it = _mime.begin(); it != _mime.end(); it++)
		std::cout << "\t" << it->first << "\t" << it->second << std::endl;
	std::cout << "}\n";

	int idx = 1;
	for (std::vector<struct s_serv>::const_iterator it = _serv.begin(); it != _serv.end(); it++)
	{
		struct s_serv temp = *it;
		std::cout << "server" << idx << " {\n";

		std::cout << "\t" << "listen " << temp.port << std::endl;
		std::cout << "\t" << "client_max_body_size " << temp.maxSize << std::endl;
		std::cout << "\t" << "root " << temp.root << std::endl;
		std::cout << "\t" << "server_name " << temp.name << std::endl;
		
		for (std::map<long, std::string>::const_iterator eit = temp.error.begin(); eit != temp.error.end(); eit++)
			std::cout << "\t" << "error_page " << eit->first << " " << eit->second << std::endl;

		for (std::map<std::string, struct s_location>::const_iterator pit = temp.path.begin(); pit != temp.path.end(); pit++)
		{
			std::cout << "\t" << "location " << pit->first << " {\n";

			std::string t_path = pit->first;
			struct s_location t_loca = pit->second;
			if (t_path.front() == '/')
			{
				for (std::vector<std::string>::iterator iit = t_loca.index.begin(); iit != t_loca.index.end(); iit++)
					std::cout << "\t\t" << "index " << *iit << std::endl;	
				std::cout << "\t\t" << "autoindex " << t_loca.autoindex << std::endl;
			}
			else
				std::cout << "\t\t" << "cgi_pass " << t_loca.cgiPass << std::endl;
			std::cout << "\t}\n";
		}
		std::cout << "}\n";
		idx++;
	}
}