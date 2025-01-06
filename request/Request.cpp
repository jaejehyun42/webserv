#include "Request.hpp"

Request::Request() {}

void Request::initRequest(ifstream& file)
{
	string line;

	if (!getline(file, line))
		throw runtime_error("Error: Empty status line");
	_parseStatus(line); // 상태 라인 파싱

	while (getline(file, line)) // 헤더 파싱 루프
	{
		if (line.empty()) // 헤더와 바디를 구분하는 빈 줄 체크
			break ;
		_parseHeader(line);
	}

	while (getline(file, line)) // 바디 초기화
	{
		_body += line;
		_body += "\n";
	}
}

Request::~Request() {}

void Request::_parseUrl()
{
	if (!_url.empty() && _url[0] != '/') // Url이 존재하는데 / 로 시작하지 않는다면 오류
		throw runtime_error("Error: Wrong url");

	size_t pos = _url.find('?'); // 쿼리 구분자 체크

	if (pos == string::npos)
	{
		_path = _url;
	}
	else
	{
		_path = _url.substr(0, pos);
		_query = _url.substr(pos + 1, _url.size() - 1);
	}
}

void Request::_parseStatus(string line)
{
	if (count(line.begin(), line.end(), ' ') != 2) // 상태 라인에 공백이 2개 이상 있으면 오류
		throw runtime_error("Error: Wrong status line form (space)");

	istringstream stream(line);

	stream >> _method;
	if (_method != "GET" && _method != "POST" && _method != "DELETE") // 과제에 명시된 메서드인지 체크
		throw runtime_error("Error: Wrong method");

	stream >> _url;
	_parseUrl();

	stream >> _version;
	if (_version != "HTTP/1.1") // 1.1버전인지 체크
		throw runtime_error("Error: Wrong version");
}

void Request::_parseHeader(string line)
{
	size_t delimiter_pos = line.find(':'); // key 와 value의 구분자인 ':'을 체크

	if (delimiter_pos == string::npos)
		throw runtime_error("Error: No delimiter found in header");

	string key = line.substr(0, delimiter_pos);

	size_t value_pos = line.find_first_not_of(" \t", delimiter_pos + 1); // ':' 이후 공백을 무시

	if (value_pos == string::npos)
		throw runtime_error("Error: Wrong header value");

	string value = line.substr(value_pos, line.size() - 1);

	_headers[key] = value; // key, value 쌍 삽입
}

string Request::getMethod()
{
	return (_method);
}

string Request::getUrl()
{
	return (_url);
}

string Request::getPath()
{
	return (_path);
}
string Request::getQuery()
{
	return (_query);
}

string Request::getVersion()
{
	return (_version);
}

unordered_map<string, string> Request::getHeaders()
{
	return (_headers);
}

string Request::getBody()
{
	return (_body);
}
