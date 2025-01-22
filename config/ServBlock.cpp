#include "ServBlock.hpp"
#include "utils_conf.hpp"

ServBlock::ServBlock()
{
	_maxSize = 1024;
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
		{
			char* end;
			long port = strtol(value.c_str(), &end, 10);
			if (port < 0 || *end != '\0')
				throw runtime_error("Error: Server: 구성 요소의 값이 잘못 되었습니다.");
			_port = value;
		}
		else if (key == "client_max_body_size")
		{
			char* end;
			_maxSize = strtol(value.c_str(), &end, 10);
			if (_maxSize < 0 || *end != '\0')
				throw runtime_error("Error: Server: 구성 요소의 값이 잘못 되었습니다.");
		}
		else if (key == "root")
		{
			if (value.back() == '/' && value.size() != 1)
				value.pop_back();
			_root = value;
		}
		else if (key == "server_name")
			_name.push_back(value);
		else if (key == "return")
		{
			char* end;
			long code = strtol(value.c_str(), &end, 10);
			if (code < 0 || *end != '\0')
				throw runtime_error("Error: Server: 구성 요소의 값이 잘못 되었습니다.");
			_return.first = value;

			char& temp = _return.first.front();
			if (_return.first.size() != 3)
				throw runtime_error("Error: Server: 지원하는 서버 옵션이 아닙니다.");
			if (temp != '2' && temp != '3' && temp != '4' && temp != '5')
				throw runtime_error("Error: Server: 지원하는 서버 옵션이 아닙니다.");
		}
		else
			throw runtime_error("Error: Server: 지원하는 서버 옵션이 아닙니다.");
	}
	else if (tokens.size() > 2)
	{
		if (key == "error_page")
		{
			for (vector<string>::iterator it = tokens.begin() + 1; it != tokens.end() - 1; it++)
			{
				char* end;
				long code = strtol((*it).c_str(), &end, 10);
				if (code < 0 || *end != '\0')
					throw runtime_error("Error: Server: 구성 요소의 값이 잘못 되었습니다.");
				_error[code] = value;
			}
		}
		else if (key == "server_name")
		{
			for (vector<string>::iterator it = tokens.begin() + 1; it != tokens.end() - 1; it++)
				_name.push_back(*it);
			_name.push_back(value);
		}
		else if (key == "return")
		{
			char* end;
			long code = strtol(tokens[1].c_str(), &end, 10);
			if (code < 0 || *end != '\0')
				throw runtime_error("Error: Server: 구성 요소의 값이 잘못 되었습니다.");

			_return.first = tokens[1];
			_return.second = tokens[2];
			if (_return.first.front() == '4' || _return.first.front() == '5')
				throw runtime_error("Error: Server: 지원하는 서버 옵션이 아닙니다.");

			char& temp = _return.first.front();
			if (tokens.size() > 3 || _return.first.size() != 3)
				throw runtime_error("Error: Server: 지원하는 서버 옵션이 아닙니다.");
			if (temp != '2' && temp != '3' && temp != '4' && temp != '5')
				throw runtime_error("Error: Server: 지원하는 서버 옵션이 아닙니다.");
		}
		else
			throw runtime_error("Error: Server: 블록 포멧이 잘못 되었습니다");
	}
	else
		throw runtime_error("Error: Server: 지원하는 서버 옵션이 아닙니다.");
}

void ServBlock::_parseBlock(vector<string>& tokens, ifstream& file)
{
	string& key = tokens.front();

	if (key == "location")
	{
		if (tokens.size() == 2)
			throw runtime_error("Error: Server: location 블록 포멧이 잘못 되었습니다");

		LocBlock lb;
		string& path = *(tokens.begin() + 1);
		lb.parseLocBlock(file, path);

		if (path.back() == '/' && path.size() != 1)
			path.pop_back();
		else if (path.front() == '.' && path.back() == '$')
		{
			if (path.find("py") == std::string::npos)
				throw runtime_error("Error: Server: 구성 요소의 값이 잘못 되었습니다.");
			path.pop_back();
		}
		_path[path] = lb;
	}
	else
		throw runtime_error("Error: Server: 지원하는 서버 옵션이 아닙니다.");
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
			throw runtime_error("Error: Server: 블록 포멧이 잘못 되었습니다");
	}
	if (line.find("}") == string::npos)
		throw runtime_error("Error: Server: 블록 포멧이 잘못 되었습니다.");
	if (_port == "" || _maxSize <= 0)
		throw runtime_error("Error: Server: 설정 파일의 필수 구성 요소가 없습니다.");
	if (_path.size() == 0)
		throw runtime_error("Error: Server: 설정 파일의 필수 구성 요소가 없습니다.");
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

const pair<string, string>& ServBlock::getReturn() const
{
	return (_return);
}

const map<long, string>& ServBlock::getErrorPage() const
{
	return (_error);
}

const map<string, LocBlock>& ServBlock::getPath() const
{
	return (_path);
}

const map<std::string, LocBlock>::const_iterator ServBlock::getPathIter(const std::string& path) const
{
	std::string current = path;

	while (1)
	{
		map<std::string, LocBlock>::const_iterator it = _path.find(current);
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
