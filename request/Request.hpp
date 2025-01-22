#pragma once

#include <iostream>
#include <fstream>
#include <map>
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
		string _scriptPath;
		string _cgiPath;
		string _version;
		map<string, string> _headers;	
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
		void _unchunk(istringstream& form);
		void _setError(int error_code);

	public :
		Request();
		~Request();
		void initRequest(const string& input);

		string getMethod() const;
		string getUrl() const;
		string getPath() const;
		string getQuery() const;
		string getScriptPath() const;
		string getCgiPath() const;
		string getVersion() const;
		map<string, string> getHeaders() const;
		string getBody() const;
		string getErrorMessage() const;
		string getErrorCode() const;

		bool chkConnection() const;
		
		typedef map<string, string>::const_iterator umap_it;
};
