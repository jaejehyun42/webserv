#include "ServBlock.hpp"
#include "utils_conf.hpp"

ServBlock::ServBlock()
{
	_port = 0;
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
			_port = strtol(value.c_str(), NULL, 10);
		else if (key == "client_max_body_size")
			_maxSize = strtol(value.c_str(), NULL, 10);
		else if (key == "root")
			_root = value;
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
	if (_port == 0 || _maxSize == 0)
		throw runtime_error("Error: 설정 파일의 필수 구성 요소가 없습니다.");
	if (_error.size() == 0 || _path.size() == 0)
		throw runtime_error("Error: 설정 파일의 필수 구성 요소가 없습니다.");
}

// getter 함수
int ServBlock::getPort() const
{
	return (_port);
}

int ServBlock::getMaxSize() const
{
	return (_maxSize);
}

string ServBlock::getRoot() const
{
	return (_root);
}

vector<string> ServBlock::getName() const
{
	return (_name);
}

string ServBlock::getErrorPage(int status) const
{
	unordered_map<long, string>::const_iterator cit = _error.find(status);
	
	if (cit != _error.end())
		return (cit->second);
	return ("");
}

// 임시
void ServBlock::print() const
{
	cout << "Server"<< " {\n";

	cout << "\t" << "listen " << _port << endl;
	cout << "\t" << "client_max_body_size " << _maxSize << endl;
	cout << "\t" << "root " << _root << endl;

	for (vector<string>::const_iterator it = _name.begin(); it != _name.end(); it++)
		cout << "\t" << "server_name " << *it << endl;
	
	for (unordered_map<long, string>::const_iterator it = _error.begin(); it != _error.end(); it++)
		cout << "\t" << "error_page " << it->first << " " << it->second << endl;

	cout << endl;
	for (unordered_map<string, LocBlock>::const_iterator it = _path.begin(); it != _path.end(); it++)
		it->second.print(it->first);

	cout << "}\n";
}