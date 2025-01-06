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
			_root = value;
		else if (key == "autoindex" && value == "on")
			_autoindex = true;
		else if (key == "index")
		{
			for (vector<string>::iterator it = tokens.begin() + 1; it != tokens.end(); it++)
				_index.push_back(*it);
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
					check[GET] = 1;
				else if (*it == "POST")
					check[POST] = 1;
				else if (*it == "DELETE")
					check[DELETE] = 1;
				else
					throw runtime_error("Error: 지원하는 서버 옵션이 아닙니다.");
			}
			if (check[GET] == 0)
				throw runtime_error("Error: 지원하는 서버 옵션이 아닙니다.");
			if (check[POST] == 0)
				throw runtime_error("Error: 지원하는 서버 옵션이 아닙니다.");
			if (check[DELETE] == 0)
				_method[DELETE] = 0;
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
bool LocBlock::getAutoindex() const
{
	return (_autoindex);
}

bool LocBlock::getMethod(int method) const
{
	return (_method[method]);
}

string LocBlock::getRoot() const
{
	return (_root);
}

string LocBlock::getCgipass() const
{
	return (_cgiPass);
}

vector<string> LocBlock::getIndex() const
{
	return (_index);
}

// 임시
void LocBlock::print(string path) const
{
	cout << "\t" << "location " << path << " {\n";

	if (path.front() == '/')
	{
		if (_root != "")
			cout << "\t\t" << "root: " << _root << endl;
		for (vector<string>::const_iterator it = _index.begin(); it != _index.end(); it++)
			cout << "\t\t" << "index " << *it << endl;	
		if (_autoindex == true)
			cout << "\t\t" << "autoindex: on\n";
		else
			cout << "\t\t" << "autoindex: off\n";
	}
	else
		cout << "\t\t" << "cgi_pass " << _cgiPass << endl;

	cout << "\t}\n";
}