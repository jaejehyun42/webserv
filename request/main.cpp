#include "Request.hpp"

void printRequest(Request re)
{
	cout << "Method = " << re.getMethod() << "\n";
	cout << "URL = " << re.getUrl() << "\n";
	cout << "Path = " << re.getPath() << "\n";
	cout << "Query = " << re.getQuery() << "\n";
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

	Request re;
	re.initRequest(file); // 파싱, 초기화

	printRequest(re); // 초기화가 잘 되었는지 체크

	return (0);
}
