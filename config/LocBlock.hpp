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
class LocBlock
{
private:
	bool _autoindex;
	bool _method[3]; // 인덱스 순서는 GET, POST, DELETE 순서
	string _root;
	string _cgiPass;
	vector<string> _index;

	void _parseLine(vector<string>& tokens);
public:
	LocBlock();
	~LocBlock();

	void parseLocBlock(ifstream& file, const string& path);

	bool getAutoindex() const;
	bool getMethod(int method) const;
	string getRoot() const;
	string getCgipass() const;
	vector<string> getIndex() const;

	void print(string path) const; // 임시
};

#endif