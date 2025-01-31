#ifndef SERVBLOCK_HPP
# define SERVBLOCK_HPP

#include <vector>
#include <map>
#include <fstream>
#include <iostream>
#include <exception>
#include "LocBlock.hpp"

using namespace std;

// Server 블록 관련
// 서버 포트, 클라이언트 바디 사이즈, location block 정의 안 되어 있는 경우 에러 처리
class ServBlock
{
private:
	int _maxSize;							// 클라이언트 최대 바디 사이즈
	string _port;							// 서버 포트 넘버
	string _root;							// root 위치 정보
	vector<string> _name;					// 서버 이름 정보
	pair<string, string> _return;			// 리다이렉션 정보
	map<long, string> _error;		// key: status, value: page
	map<string, LocBlock>_path;	// path에 따른 location 정보

	void _parseLine(vector<string>& tokens);
	void _parseBlock(vector<string>& tokens, ifstream& file);
public:
	ServBlock();
	~ServBlock();

	void parseServBlock(ifstream& file);

	const int& getMaxSize() const;
	const string& getPort() const;
	const string& getRoot() const;
	const vector<string>& getName() const;
	const pair<string, string>& getReturn() const;

	const map<long, string>& getErrorPage() const;
	const map<string, LocBlock>& getPath() const;
	const map<string, LocBlock>::const_iterator getPathIter(const string& path) const;
};

#endif
