#ifndef SERVCONF_HPP
# define SERVCONF_HPP

#include <vector>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <exception>
#include "ServBlock.hpp"

using namespace std;

// 전체적인 내용 및 http 블록 관련 클래스
class ServConf
{
private:
	long _aliveTime;
	vector<ServBlock> _serv;
	unordered_map<string, string> _mime;

	void _includeFile(const string& fileName);
	void _parseMime(ifstream& file);
	void _parseHTTP(ifstream& file, bool inc); // include를 통해서 들어간 경우 true, 그 외에 false
public:
	ServConf();
	~ServConf();

	void parse(const string& fileName);

	const long& getAliveTime() const;
	const ServBlock& getServBlock(int idx) const;
	const vector<ServBlock>& getServ() const;
	const string& getMime(const string& key) const;
};

#endif