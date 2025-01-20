#include "Request.hpp"

Request::Request() {}

Request::~Request() {}

void Request::_setError(int error_code)
{
	if (error_code == 400)
	{
		_errorCode = "400";
		_errorMessage = "Bad Request";
	}
	else if (error_code == 505)
	{
		_errorCode = "505";
		_errorMessage = "HTTP Version Not Supported";
	}
	else if (error_code == 405)
	{
		_errorCode = "405";
		_errorMessage = "Method Not Allowed";
	}

	throw runtime_error(_errorCode + _errorMessage);
}

// parse

void Request::_parseMethod()
{
	for (size_t i = 0; i < _method.size(); ++i) // 메서드는 전부 대문자여야함
	{
		if (!isupper(_method[i]))
			_setError(400);
	}
}

void Request::_parseKey(const string& key)
{
	for (size_t i = 0; i < key.size(); ++i) // key는 공백이 없어야함
	{
		if (isspace(key[i]))
			_setError(400);
	}
}

void Request::_parseHost(const string& value)
{
		if (_headers.find("Host") != _headers.end()) // Host 헤더는 2개 이상일 수 없음
			_setError(400);

		istringstream stream(value);

		string str;
		stream >> str;
		if (stream >> str) // Host 헤더에는 value가 2개 이상일 수 없음
			_setError(400);
}

void Request::_parseMethodChkHost()
{
	if (_headers.find("Host") == _headers.end()) // Host 헤더가 존재하지 않음
		_setError(400);
	else if (_method != "GET" && _method != "POST" && _method != "DELETE")
		_setError(405);
}

void Request::initRequest(const string& input)
{
	istringstream form(input);
	string line;

	try
	{
		if (!getline(form, line))
			_setError(400);
		_parseStatus(line); // 상태 라인 파싱
	
		while (getline(form, line)) // 헤더 파싱 루프
		{
			if (line == "\r") // 헤더와 바디를 구분하는 빈 줄 체크
				break ;
			_parseHeader(line);
		}
		_parseMethodChkHost();
	
		while (getline(form, line)) // 바디 초기화
			_body += line + "\n";
	}
	catch (exception& e)
	{
		return;
	}
}

void Request::_parseUrl()
{
	if (!_url.empty() && _url[0] != '/') // Url이 존재하는데 / 로 시작하지 않는다면 오류
		_setError(400);

	int start = 0;
	for (int i = 0; _url[i] == '/'; ++i) // Url 시작이 '/'이 연속되면 하나 빼고 무시
		start++;
	start--;
	
	size_t end_pos = _url.size() - 1;
	if (end_pos && _url[end_pos] == '/') // 마지막 '/' 삭제
		_url.erase(end_pos, 1);

	size_t delim_pos = _url.find('?'); // 쿼리 구분자 체크

	if (delim_pos == string::npos)
	{
		_path = _url.substr(start, delim_pos);
	}
	else
	{
		_path = _url.substr(start, delim_pos - start);
		_query = _url.substr(delim_pos + 1, _url.size() - delim_pos); 
	}

	size_t cgi_pos = _path.find(".py"); // cgi 경로 체크
	if (cgi_pos != string::npos)
		_cgiPath = _path.substr(cgi_pos + 3, _path.size() - cgi_pos - 2);

	if (!_cgiPath.empty()) // 스크립트 경로 체크
		_scriptPath = _path.substr(0, cgi_pos + 3);
}

void Request::_parseVersion()
{
	size_t delim_pos = _version.find('/'); // 구분자 '/' 체크

	string HTTP = _version.substr(0, delim_pos); // HTTP 체크
	if (HTTP != "HTTP")
		_setError(400);

	string version_num = _version.substr(delim_pos + 1, _version.size() - delim_pos);

	char *endptr;
	double version_num_doub = strtod(version_num.c_str(), &endptr);
	if (endptr == version_num.c_str()) // 숫자가 아니라면 400
		_setError(400);
	else if (version_num_doub != 1.1) // 버전 숫자 체크
		_setError(505);

	if (_version != "HTTP/1.1") // 마지막 체크
		_setError(400);
}

void Request::_parseStatus(const string& line)
{
	if (isspace(line[0])) // 시작이 공백인지 체크
		_setError(400);
	for (size_t i = 0; i < line.size(); ++i) // 상태 라인은 탭 사용 불가
	{
		if (line[i] == '\t')
			_setError(400);
	}

	istringstream stream(line);

	stream >> _method;
	_parseMethod();

	stream >> _url;
	_parseUrl();

	stream >> _version;
	_parseVersion();

	string spare;
	stream >> spare;
	if (!spare.empty()) // 버전 이후 남은 문자열 체크
		_setError(400);
}

void Request::_parseHeader(const string& line)
{
	size_t delim_pos = line.find(':'); // 구분자 ':' 위치 체크

	if (delim_pos == string::npos)
		_setError(400);

	string key = line.substr(0, delim_pos);
	_parseKey(key);

	size_t value_pos = line.find_first_not_of(" ", delim_pos + 1); // ':' 이후 공백을 무시

	if (line[value_pos] == ':') // value의 시작이 ':' 이면 오류
		_setError(400);
	else if (value_pos == string::npos)
		_setError(400);

	string value = line.substr(value_pos, line.size() - value_pos - 1);

	for (size_t i = 0; i < key.size(); ++i) // key 대소문자 변환
	{
		if (i && key[i - 1] != '-')
			key[i] = tolower(key[i]);
		else
			key[i] = toupper(key[i]);
	}

	if (key == "Host")
		_parseHost(value);

	_headers[key] = value;
}

// utility

bool Request::chkConnection() const
{
	umap_it it = _headers.find("Connection");

	if (it != _headers.end() && it->second == "Closed")
		return (true);
	return(false);
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

string Request::getScriptPath() const
{
	return (_scriptPath);
}

string Request::getCgiPath() const
{
	return (_cgiPath);
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

string Request::getErrorMessage() const
{
	return (_errorMessage);
}

string Request::getErrorCode() const
{
	return (_errorCode);
}
