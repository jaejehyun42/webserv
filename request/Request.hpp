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

		void _parseStatus(const string& line);
		void _parseUrl();
		void _parseHeader(const string& line);

	public :
		Request();
		~Request();
		void initRequest(ifstream& file);

		string getMethod() const;
		string getUrl() const;
		string getPath() const;
		string getQuery() const;
		string getVersion() const;
		unordered_map<string, string> getHeaders() const;
		string getBody() const;
};
