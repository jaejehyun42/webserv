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
// ServBlock나 default_type이 정의 안 되어 있는 경우 에러 처리
class ServConf
{
private:
	long _aliveTime;
	vector<ServBlock> _serv;
	unordered_map<string, string> _mime;

	void _includeFile(const string& fileName);
	void _parseMime(ifstream& file);
	void _parseHTTP(ifstream& file, bool inc); // include를 통해서 들어간 경우 true, 그 외에 false
	void _parse(ifstream& file);
public:
	ServConf(const string& fileName);
	~ServConf();

	const long& getAliveTime() const;
	const ServBlock& getServBlock(size_t idx) const;
	const vector<ServBlock>& getServ() const;
	const string& getMime(const string& key) const;
};

#endif