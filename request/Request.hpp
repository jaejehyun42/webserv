#pragma once

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <algorithm>
#include <iterator>
#include <string>
#include <stdexcept>
#include <sstream>
#include <cctype>
#include <cstdlib>

using namespace std;

class Request
{
	private :
		string _method;
		string _url;
		string _path;
		string _query;
		string _cgiPath;
		string _version;
		unordered_map<string, string> _headers;	
		string _body;
		string _errorMessage;
		string _errorCode;

		void _parseStatus(const string& line);
		void _parseUrl();
		void _parseHeader(const string& line);
		void _parseVersion();
		void _parseKey(const string& key);
		void _parseHost(const string& value);
		void _parseMethod();
		void _parseMethodChkHost();
		void _setError(int error_code);

	public :
		Request();
		~Request();
		void initRequest(const string& input);

		string getMethod() const;
		string getUrl() const;
		string getPath() const;
		string getQuery() const;
		string getCgiPath() const;
		string getVersion() const;
		unordered_map<string, string> getHeaders() const;
		string getBody() const;
		string getErrorMessage() const;
		string getErrorCode() const;

		bool chkConnection() const;
		
		typedef unordered_map<string, string>::const_iterator umap_it;
};
