#include "Request.hpp"

Request::Request() {}

void Request::initRequest(ifstream& file)
{
	string line;

	if (!getline(file, line))
		throw runtime_error("400");
	_parseStatus(line); // 상태 라인 파싱

	while (getline(file, line)) // 헤더 파싱 루프
	{
		if (line.empty()) // 헤더와 바디를 구분하는 빈 줄 체크
			break ;
		_parseHeader(line);
	}

	while (getline(file, line)) // 바디 초기화
		_body += line + "\n";
}

Request::~Request() {}

void Request::_parseUrl()
{
	if (!_url.empty() && _url[0] != '/') // Url이 존재하는데 / 로 시작하지 않는다면 오류
		throw runtime_error("400");

	int start = 0;
	for (int i = 0; _url[i] == '/'; ++i) // Url 시작이 '/'이 연속되면 하나 빼고 무시
		start++;
	start--;

	size_t delim_pos = _url.find('?'); // 쿼리 구분자 체크

	if (delim_pos == string::npos)
	{
		_path = _url.substr(start, delim_pos);
	}
	else
	{
		_path = _url.substr(start, delim_pos - start);
		_query = _url.substr(delim_pos + 1, _url.size() - 1); 
	}
}

void Request::_parseVersion()
{
	size_t delim_pos = _version.find('/'); // 구분자 '/' 체크

	string HTTP = _version.substr(0, delim_pos); // HTTP 체크
	if (HTTP != "HTTP")
		throw runtime_error("400");

	string version_num = _version.substr(delim_pos + 1, _version.size() - 1);

	char *endptr;
	double version_num_doub = strtod(version_num.c_str(), &endptr);
	if (endptr == version_num.c_str()) // 숫자가 아니라면 400
		throw runtime_error("400");
	else if (version_num_doub != 1.1) // 버전 숫자 체크
		throw runtime_error("505");

	if (_version != "HTTP/1.1") // 마지막 체크
		throw runtime_error("400");
}

void Request::_parseStatus(const string& line)
{
	if (isspace(line[0])) // 시작이 공백인지 체크
		throw runtime_error("400");

	istringstream stream(line);

	stream >> _method;
	if (_method != "GET" && _method != "POST" && _method != "DELETE") // 과제에 명시된 메서드인지 체크
		throw runtime_error("405");

	stream >> _url;
	_parseUrl();

	stream >> _version;
	_parseVersion();

	string spare;
	stream >> spare;
	if (!spare.empty()) // 버전 이후 남은 문자열 체크
		throw runtime_error("400");
}

void Request::_parseHeader(const string& line)
{
	if (count(line.begin(), line.end(), ':') != 1) // ':' 구분자가 1개 이상 있으면 오류 
		throw runtime_error("400");

	size_t delim_pos = line.find(':'); // 구분자 ':' 위치 체크

	if (delim_pos == string::npos)
		throw runtime_error("400");

	string key = line.substr(0, delim_pos);

	size_t value_pos = line.find_first_not_of(" \t", delim_pos + 1); // ':' 이후 공백을 무시

	if (value_pos == string::npos)
		throw runtime_error("400");

	string value = line.substr(value_pos, line.size() - 1);

	_headers[key] = value; // key, value 쌍 삽입
}

string Request::getMethod() const
{
	return (_method);
}

string Request::getUrl() const
{
	return (_url);
}

string Request::getPath() const
{
	return (_path);
}
string Request::getQuery() const
{
	return (_query);
}

string Request::getVersion() const
{
	return (_version);
}

unordered_map<string, string> Request::getHeaders() const
{
	return (_headers);
}

string Request::getBody() const
{
	return (_body);
}
