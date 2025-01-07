#ifndef SERVBLOCK_HPP
# define SERVBLOCK_HPP

#include <vector>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <exception>
#include "LocBlock.hpp"

using namespace std;

/// Server 블록 관련
class ServBlock
{
private:
	int _port;								// 서버 포트 넘버
	int _maxSize;							// 클라이언트 최대 바디 사이즈
	string _root;							// root 위치 정보
	vector<string> _name;					// 서버 이름 정보
	unordered_map<long, string> _error;		// key: status, value: page
	unordered_map<string, LocBlock>_path;	// path에 따른 location 정보

	void _parseLine(vector<string>& tokens);
	void _parseBlock(vector<string>& tokens, ifstream& file);
public:
	ServBlock();
	~ServBlock();

	void parseServBlock(ifstream& file);

	const int& getPort() const;
	const int& getMaxSize() const;
	const string& getRoot() const;
	const vector<string>& getName() const;
	const string& getErrorPage(int status) const;
	const LocBlock& getLocation(string path) const;

	void print() const; // 임시
};

#endif