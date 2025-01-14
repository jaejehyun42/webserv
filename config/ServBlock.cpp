#include "ServBlock.hpp"
#include "utils_conf.hpp"

ServBlock::ServBlock()
{
	_maxSize = 0;
}

ServBlock::~ServBlock() {}

void ServBlock::_parseLine(vector<string>& tokens)
{
	string& key = tokens.front();
	string& value = tokens.back();
	value.pop_back();

	if (tokens.size() == 2)
	{
		if (key == "listen")
			_port = value;
		else if (key == "client_max_body_size")
		{
			char* end;
			_maxSize = strtol(value.c_str(), &end, 10);
			if (_maxSize < 0 || *end != '\0')
				throw runtime_error("Error: 구성 요소의 값이 잘못 되었습니다.");
		}
		else if (key == "root")
		{
			if (value.back() == '/' && value.size() != 1)
				value.pop_back();
			_root = value;
		}
		else if (key == "server_name")
			_name.push_back(value);
		else
			throw runtime_error("Error: 지원하는 서버 옵션이 아닙니다.");
	}
	else if (tokens.size() > 2)
	{
		if (key == "error_page")
		{
			for (vector<string>::iterator it = tokens.begin() + 1; it != tokens.end() - 1; it++)
				_error[strtol((*it).c_str(), NULL, 10)] = value;
		}
		else if (key == "server_name")
		{
			for (vector<string>::iterator it = tokens.begin() + 1; it != tokens.end() - 1; it++)
				_name.push_back(*it);
			_name.push_back(value);
		}
		else
			throw runtime_error("Error: server 블록 포멧이 잘못 되었습니다");
	}
	else
		throw runtime_error("Error: 지원하는 서버 옵션이 아닙니다.");
}

void ServBlock::_parseBlock(vector<string>& tokens, ifstream& file)
{
	string& key = tokens.front();

	if (key == "location")
	{
		if (tokens.size() == 2)
			throw runtime_error("Error: location 블록 포멧이 잘못 되었습니다");

		LocBlock lb;
		string& path = *(tokens.begin() + 1);
		lb.parseLocBlock(file, path);

		if (path.back() == '/' && path.size() != 1)
			path.pop_back();
		else if (path.front() == '.' && path.back() == '$')
			path.pop_back();
		_path[path] = lb;
	}
	else
		throw runtime_error("Error: 지원하는 서버 옵션이 아닙니다.");
}

void ServBlock::parseServBlock(ifstream& file)
{
	string line;
	
	while (getline(file, line))
	{
		vector<string> tokens = splitString(line);
		if (tokens.size() == 0)
			continue ;
		if (tokens.front().front() == '#')
			continue ;

		string& delim = tokens.back();
		if (delim == "}" && tokens.size() == 1)
			break ;
		else if (delim == "{")
			_parseBlock(tokens, file);
		else if (delim.back() == ';')
			_parseLine(tokens);
		else
			throw runtime_error("Error: server 블록 포멧이 잘못 되었습니다");
	}
	if (line.find("}") == string::npos)
		throw runtime_error("Error: server 블록 포멧이 잘못 되었습니다.");
	if (_port == "" || _maxSize == 0)
		throw runtime_error("Error: 설정 파일의 필수 구성 요소가 없습니다.");
	if (_path.size() == 0)
		throw runtime_error("Error: 설정 파일의 필수 구성 요소가 없습니다.");
}

// getter 함수
const int& ServBlock::getMaxSize() const
{
	return (_maxSize);
}

const string& ServBlock::getPort() const
{
	return (_port);
}

const string& ServBlock::getRoot() const
{
	return (_root);
}

const vector<string>& ServBlock::getName() const
{
	return (_name);
}

const unordered_map<long, string>& ServBlock::getErrorPage() const
{
	return (_error);
}

const unordered_map<string, LocBlock>& ServBlock::getPath() const
{
	return (_path);
}

const unordered_map<std::string, LocBlock>::const_iterator ServBlock::getPathIter(const std::string& path) const
{
	std::string current = path;

	while (1)
	{
		unordered_map<std::string, LocBlock>::const_iterator it = _path.find(current);
		if (it != _path.end())
			return (it);

		std::size_t pos = current.find_last_of('/');
		if (pos == std::string::npos || current == "/")
			break ;
		if (pos == 0)
			current = "/";
		else
			current = current.substr(0, pos);
	}
	return (_path.end());
}
