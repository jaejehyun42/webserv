#include "Request.hpp"

void printRequest(Request re)
{
	cout << "Method = " << re.getMethod() << "\n";
	cout << "URL = " << re.getUrl() << "\n";
	cout << "Path = " << re.getPath() << "\n";
	cout << "Query = " << re.getQuery() << "\n";
	cout << "ScriptPath = " << re.getScriptPath() << "\n";
	cout << "CgiPath = " << re.getCgiPath() << "\n";
	cout << "Version = " << re.getVersion() << "\n";
	unordered_map<string, string> headers = re.getHeaders();
	cout << "Headers = " << "\n";
	for (Request::umap_it it = headers.begin(); it != headers.end(); ++it)
		cout << it->first << ": " << it->second << "\n";
	cout << "Body = " << "\n" << re.getBody() << endl;
}

// 프로그램 인수로 Request.http
int main(int argc, char** argv)
{
	if (argc != 2)
	{
		cout << "Error: HTTP Message required, argument needed" << endl;
		return (1);
	}

	ifstream file(argv[1]);
	if (!file.is_open())
	{
		cout << "Error: could not open file" << endl;
		exit(1);
	}

	string input = "GET /get/cgi-bin/my_cgi.py/foo/bar?key1=value&key2=value HTTP/1.1\r\n"
				  "User-Agent: PostmanRuntime/7.32.3\r\n"
				  "Accept: */*\r\n"
				  "Postman-Token: bda49f4f-8500-4559-a291-9bffaa08cd17\r\n"
				  "Host: postman-echo.com\r\n"
				  "Accept-Encoding: gzip, deflate, br\r\n"
				  "Connection: keep-alive\r\n"
				  "\r\n"
				  "{\n"
				  "\"args\": {},\n"
				  "\"host\": \"postman-echo.com\",\n"
				  "\"x-request-start\": \"t1735812866.422\",\n"
				  "}\n";

	Request re;
	re.initRequest(input); // 파싱, 초기화

	printRequest(re); // 초기화가 잘 되었는지 체크

	return (0);
}
