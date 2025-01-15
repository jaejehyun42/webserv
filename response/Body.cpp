#include <string>
#include <iostream>
#include <fstream>
#include <sys/socket.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <cstdlib>
#include <vector>
#include "Body.hpp"
#include "ResponseManager.hpp"

Body::Body(std::unordered_map<int, std::string>& data): _data(data){
	_setMessage();
}

Body::~Body(){}

std::string  Body::getMessage(){
	return (_message);
}
void        Body::_setMessage(){
	if (_data.at(__requestMethod) == "GET")
		_makeGetMessage();
	else if (_data.at(__requestMethod) == "POST")
		_makePostMessage();
	else if (_data.at(__requestMethod) == "DELETE")
		_makeDeleteMessage();
	else
		throw(std::runtime_error("405"));
}

void    Body::_makeAutoindexMessage(){
	DIR* dir = opendir(_data.at(__path).c_str());
	struct dirent   *entry;
	std::string     relativePath; //e.g test_dir
	std::string     currentPath = _data.at(__path); //e.g Users/goinfre/test_dir
	std::string     root = _data.at(__root); //e.g Users/goinfre
	// std::string		entryFileName;
	int savedErrno = errno;

	if (dir == NULL){
		errno = savedErrno;
		throw(std::runtime_error("500"));
	}
	if (currentPath.size() > root.size())
		relativePath = currentPath.erase(0, root.size());
	else if (currentPath.size() == root.size() && _data.find(__locationIdentifier) != _data.end())
		relativePath = _data.at(__locationIdentifier);
	_message += 
				"<!DOCTYPE html>\n"
				"<html>\n"
				"<head>\n"
				"<title>Index</title>\n"
				"</head>\n"
				"<body>\n"
				"	<h1>Index of " + relativePath + "</h1>\n"
				"	<hr>\n"
				"	<ul>\n";
	// if (relativePath.size())
	// 	_message += "	<h1>Index of " + relativePath + "</h1>\n";
	// _message += 
	// 			"	<hr>\n"
	// 			"	<ul>\n";
	while ((entry = readdir(dir)) != NULL){
		if (entry->d_name[0] == '.')
			continue;
		_message +=
		"		<li><a href=\"" + relativePath + "/" + entry->d_name + "\">" + entry->d_name + "</a></li>\n";
		// if (relativePath.size())
		// 	entryFileName = relativePath + "/" + entry->d_name;
		// else
		// 	entryFileName = entry->d_name;
		// _message +="		<li><a href=\"" + entryFileName + "\">" + entry->d_name + "</a></li>\n";
	}
	_message +=
			"	</ul>\n"
			"	</hr>\n"
			"</body>\n"
			"</html>";
	closedir(dir);
}

void	Body::_readCgiMessage(pid_t& cgiProc, int* pfd){
	close(pfd[1]);

	int cgiProcStatus;
	waitpid(cgiProc, &cgiProcStatus,0);
	if (WEXITSTATUS(cgiProcStatus))
		throw(std::runtime_error("500"));

	char buf[1024];
	std::string cgiMessage;
	while (read(pfd[0], buf, sizeof(buf)) > 0){
		cgiMessage += buf;
	}
	istringstream iss(cgiMessage);
	std::string line;
	std::string key;
	size_t i;
	std::getline(iss, line, '\n');
	i = line.find(' ');
	if (i == std::string::npos || i == 0 || i == line.size() - 1)
		throw std::runtime_error("500");
	_data[__statusCode] = line.substr(0,i);
	_data[__reasonPhrase] = line.substr(i+1);

	while (std::getline(iss, line, '\n') && line != "\n"){ 
		i = line.find(':');
		if (i == std::string::npos || i == 0 || i == line.size() - 1)
			throw std::runtime_error("500");
		key = line.substr(0,i);
		if (key == "Content-Length")
			_data[__contentLength] = line.substr(i+1);
		if (key == "Content-Type")
			_data[__contentType] = line.substr(i+1);
	}
	while (std::getline(iss, line, '\n') && line != "\n"){ 
		_message += line + "\r\n";
	}
}

void	Body::_execCgiProc(int* pfd){
	close(pfd[0]);

	if (dup2(pfd[1], STDOUT_FILENO) < 0)
		exit(EXIT_FAILURE);

	char *file = const_cast<char*>(_data.at(__cgiPass).c_str());
	char *argv[3] = {file, const_cast<char*>("my_cgi.py"), 0};

	if (_data.find(__cgiEnvData) == _data.end())
		exit(EXIT_FAILURE);
	std::istringstream  iss(_data.at(__cgiEnvData));
	std::string         token;
	std::vector<char*>  tmpEnvp;
	while (std::getline(iss,token,' '))
		tmpEnvp.push_back(const_cast<char*>(token.c_str()));
	tmpEnvp.push_back(NULL);
	char **envp = tmpEnvp.data();

	if (execve(file, argv, envp))
		exit(EXIT_FAILURE);
	exit(EXIT_SUCCESS);
}

void    Body::_makeCgiMessage(){
	int pfd[2];
	int savedErrno;
	savedErrno = errno;

	if (pipe(pfd) < 0){
		errno = savedErrno;
		throw(std::runtime_error("500"));
	}
	pid_t cgiProc = fork();
	if (cgiProc < 0){
		errno = savedErrno;
		throw(std::runtime_error("500"));
	}
	cgiProc == 0 ? _execCgiProc(pfd) : _readCgiMessage(cgiProc, pfd);
}

void	Body::_makeStaticFileMessage(){
	std::ifstream ifs(_data.at(__path), std::ios::binary);
	if (!ifs)
		throw(std::runtime_error("500"));
	ifs.seekg(0, std::ios::end);
	std::streampos size = ifs.tellg();
	ifs.seekg(0, std::ios::beg);
	std::string buf(size, '\0');
	ifs.read(&buf[0], size);
	_message += buf;
	ifs.close();
}

void    Body::_setContentLength(){
	ostringstream	oss;
	oss<<_message.size();
	_data[__contentLength] = oss.str();
}

void    Body::_makeGetMessage(){
	if (_data.find(__cgiEnvData) != _data.end() && _data.at(__cgiEnvData).find("PATH_INFO") != std::string::npos)
		_makeCgiMessage();
	else if (_data.find(__autoindex) != _data.end())
		_makeAutoindexMessage();
	else
		_makeStaticFileMessage();
	_setContentLength();
}

void    Body::_makePostMessage(){
	_makeCgiMessage();
}

void    Body::_makeDeleteMessage(){
	_makeCgiMessage();
}