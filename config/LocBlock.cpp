#include "LocBlock.hpp"
#include "utils_conf.hpp"

LocBlock::LocBlock()
{
	for (int i = 0; i < 3; i++)
		_method[i] = true;
	_autoindex = false;
}

LocBlock::~LocBlock() {}

void LocBlock::_parseLine(vector<string>& tokens)
{
	string& key = tokens.front();
	string& value = tokens.back();
	value.pop_back();

	if (tokens.size() == 2)
	{
		if (key == "cgi_pass")
			_cgiPass = value;
		else if (key == "root")
		{
			if (value.back() == '/' && value.size() != 1)
				value.pop_back();
			_root = value;
		}
		else if (key == "autoindex")
		{
			if (value == "on")
				_autoindex = true;
			else if (value == "off");
			else
				throw runtime_error("Error: 구성 요소의 값이 잘못 되었습니다.");
		}
		else if (key == "index")
		{
			_index.push_back(value);
		}
		else if (key == "return")
		{
			char* end;
			long code = strtol(value.c_str(), &end, 10);
			if (code < 0 || *end != '\0')
				throw runtime_error("Error: 구성 요소의 값이 잘못 되었습니다.");
			_return.first = value;

			char& temp = _return.first.front();
			if (_return.first.size() != 3)
				throw runtime_error("Error: 지원하는 서버 옵션이 아닙니다.");
			if (temp != '2' && temp != '3' && temp != '4' && temp != '5')
				throw runtime_error("Error: 지원하는 서버 옵션이 아닙니다.");
		}
		else
			throw runtime_error("Error: 지원하는 서버 옵션이 아닙니다.");
	}
	else if (tokens.size() > 2)
	{
		if (key == "index")
		{
			for (vector<string>::iterator it = tokens.begin() + 1; it != tokens.end(); it++)
				_index.push_back(*it);
		}
		else if (key == "limit_except")
		{
			int check[3] = {0};
			for (vector<string>::iterator it = tokens.begin() + 1; it != tokens.end(); it++)
			{
				if (*it == "GET")
					check[GET] = true;
				else if (*it == "POST")
					check[POST] = true;
				else if (*it == "DELETE")
					check[DELETE] = true;
				else
					throw runtime_error("Error: 지원하는 서버 옵션이 아닙니다.");
			}
			if (check[GET] == 0)
				_method[GET] = false;
			if (check[POST] == 0)
				_method[POST] = false;
			if (check[DELETE] == 0)
				_method[DELETE] = false;
		}
		else if (key == "return")
		{
			char* end;
			long code = strtol(tokens[1].c_str(), &end, 10);
			if (code < 0 || *end != '\0')
				throw runtime_error("Error: 구성 요소의 값이 잘못 되었습니다.");

			_return.first = tokens[1];
			_return.second = tokens[2];
			if (_return.first.front() == '4' || _return.first.front() == '5')
				throw runtime_error("Error: 지원하는 서버 옵션이 아닙니다.");

			char& temp = _return.first.front();
			if (tokens.size() > 3 || _return.first.size() != 3)
				throw runtime_error("Error: 지원하는 서버 옵션이 아닙니다.");
			if (temp != '2' && temp != '3' && temp != '4' && temp != '5')
				throw runtime_error("Error: 지원하는 서버 옵션이 아닙니다.");
		}
		else
			throw runtime_error("Error: 지원하는 서버 옵션이 아닙니다.");
	}
	else
		throw runtime_error("Error: 지원하는 서버 옵션이 아닙니다.");
}

void LocBlock::parseLocBlock(ifstream& file, const string& path)
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
		else if (delim.back() == ';')
			_parseLine(tokens);
		else
			throw runtime_error("Error: location 블록 포멧이 잘못 되었습니다");
	}
	if (line.find("}") == string::npos)
		throw runtime_error("Error: location 블록 포멧이 잘못 되었습니다.");
	if (path.front() == '.' && _index.size() > 0)
		throw runtime_error("Error: location 블록 요소가 잘못 되었습니다.");
	if (path.front() == '/' && _cgiPass != "")
		throw runtime_error("Error: location 블록 요소가 잘못 되었습니다.");
}

// getter 함수
const bool& LocBlock::getAutoindex() const
{
	return (_autoindex);
}

const bool& LocBlock::getMethod(int method) const
{
	return (_method[method]);
}

const string& LocBlock::getRoot() const
{
	return (_root);
}

const string& LocBlock::getCgipass() const
{
	return (_cgiPass);
}

const vector<string>& LocBlock::getIndex() const
{
	return (_index);
}

const pair<string, string>& LocBlock::getReturn() const
{
	return (_return);
}