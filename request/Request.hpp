#pragma once

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <algorithm>
#include <iterator>
#include <string>
#include <stdexcept>
#include <sstream>

using namespace std;

class Request
{
	private :
		string _method;
		string _url;
		string _path;
		string _query;
		string _version;
		unordered_map<string, string> _headers;	
		string _body;

		void _parseStatus(string& line);
		void _parseUrl();
		void _parseHeader(string& line);

	public :
		Request();
		~Request();
		void initRequest(ifstream& file);

		string getMethod();
		string getUrl();
		string getPath();
		string getQuery();
		string getVersion();
		unordered_map<string, string> getHeaders();
		string getBody();
};
