#include "ServConf.hpp"
#include "utils_conf.hpp"

ServConf::ServConf() {}

ServConf::~ServConf() {}

void ServConf::_includeFile(const string& fileName)
{
	ifstream file(fileName.c_str());
	if (!file)
		throw runtime_error("Error: 파일이 없거나 열 수 없습니다.");

	_parseHTTP(file, true);
}

void ServConf::_parseMime(ifstream& file)
{
	string line;
	
	while (getline(file, line))
	{
		vector<string> tokens = splitString(line);
		if (tokens.size() == 0)
			continue ;
		if (tokens.front().front() == '#')
			continue ;
		if (tokens.size() == 1)
		{
			if (tokens.back() == "}")
				break ;
			else
				throw runtime_error("Error: http 블록 포멧이 잘못 되었습니다.");
		}
		if (tokens.back().back() != ';')
			throw runtime_error("Error: http 블록 포멧이 잘못 되었습니다1");

		string& value = tokens.front();
		for (vector<string>::iterator it = tokens.begin() + 1; it != tokens.end(); it++)
		{
			string& key = *it;
			if (key.back() == ';')
				key.pop_back();
			_mime[key] = value;
		}
	}
	if (line.find("}") == string::npos)
		throw runtime_error("Error: http 블록 포멧이 잘못 되었습니다.");
}

void ServConf::_parseHTTP(ifstream& file, bool inc)
{
	string line;
	while (getline(file, line))
	{
		vector<string> tokens = splitString(line);
		if (tokens.size() == 0)
			continue ;
		if (tokens.front().front() == '#')
			continue ;
		if (tokens.size() == 1)
		{
			if (tokens.back() == "}")
				break ;
			else
				throw runtime_error("Error: http 블록 포멧이 잘못 되었습니다");
		}

		string& key = tokens.front();
		string& value = tokens.back();

		if (value == "{")
		{
			if (key == "server")
			{
				ServBlock sb;
				sb.parseServBlock(file);

				_serv.push_back(sb);
			}
			else if (key == "types")
				_parseMime(file);
			else
				throw runtime_error("Error: 지원하는 서버 옵션이 아닙니다.");
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
				throw runtime_error("Error: 지원하는 서버 옵션이 아닙니다.");
		}
		else
			throw runtime_error("Error: http 블록 포멧이 잘못 되었습니다.");
	}
	if (!inc && line.find("}") == string::npos)
		throw runtime_error("Error: http 블록 포멧이 잘못 되었습니다.");
}

void ServConf::parse(const string& fileName)
{
	ifstream file(fileName.c_str());
	if (!file)
		throw runtime_error("Error: 파일이 없거나 열 수 없습니다.");

	string line;
	while (getline(file, line))
	{
		vector<string> tokens = splitString(line);
		if (tokens.size() == 0)
			continue ;
		if (tokens.front().front() == '#')
			continue ;
		if (tokens.size() == 1)
			throw runtime_error("Error: 파일 포멧이 잘못 되었습니다.");

		string& key = tokens.front();
		string& value = tokens.back();
		if (value == "{")
		{
			if (key == "http")
				_parseHTTP(file, false);
			else
				throw runtime_error("Error: 지원하는 서버 옵션이 아닙니다.");
		}
		else if (value.back() == ';')
			throw runtime_error("Error: 지원하는 서버 옵션이 아닙니다.");
		else
			throw runtime_error("Error: 파일 포멧이 잘못 되었습니다.");
	}
	if (_serv.size() < 1 || _mime.find("default") == _mime.end())
		throw runtime_error("Error: 설정 파일의 필수 구성 요소가 없습니다.");
}

// getter 함수
const long& ServConf::getAliveTime() const
{
	return (_aliveTime);
}

const ServBlock& ServConf::getServBlock(int idx) const
{
	return (_serv[idx]);
}

const vector<ServBlock>& ServConf::getServ() const
{
	return (_serv);
}

const string& ServConf::getMime(const string& key) const
{
	unordered_map<string, string>::const_iterator cit = _mime.find(key);
	
	if (cit != _mime.end())
		return (cit->second);
	else
		throw ;
}

// 임시
void ServConf::print() const
{
	cout << "keepalive_timeout: " << _aliveTime << endl;

	cout << "type {\n";
	for (unordered_map<string, string>::const_iterator it = _mime.begin(); it != _mime.end(); it++)
		cout << "\t" << it->first << "\t" << it->second << endl;
	cout << "}\n";

	for (vector<ServBlock>::const_iterator it = _serv.begin(); it != _serv.end(); it++)
		it->print();
}