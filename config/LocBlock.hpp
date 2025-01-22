#ifndef LOCBLOCK_HPP
# define LOCBLOCK_HPP

#include <vector>
#include <fstream>
#include <iostream>
#include <exception>

using namespace std;

#define GET 0
#define POST 1
#define DELETE 2

// location 블록 관련
// path가 cgi인데 index도 같이 정의되어 있는 경우 에러 처리
// 반대로 path가 디렉토리인데 cgi_pass가 정의되어 있는 경우 에러처리
class LocBlock
{
private:
	bool _autoindex;				// autoindex on/off
	bool _method[3]; 				// 인덱스 순서는 GET, POST, DELETE 순서
	string _root;					// root 정보
	string _cgiPass;				// cgi_pass 정보
	vector<string> _index;			// index 정보
	pair<string, string> _return;	// 리다이렉션 정보

	void _parseLine(vector<string>& tokens);
public:
	LocBlock();
	~LocBlock();

	void parseLocBlock(ifstream& file, const string& path);

	const bool& getAutoindex() const;
	const bool& getMethod(int method) const;
	const string& getRoot() const;
	const string& getCgipass() const;
	const vector<string>& getIndex() const;
	const pair<string, string>& getReturn() const;
};

#endif 
