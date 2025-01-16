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
				"	<ul>\n"
				"		<li><a href=\"..\">..</a></li>\n";
	while ((entry = readdir(dir)) != NULL){
		if (entry->d_name[0] == '.')
			continue;
		_message +=
		"		<li><a href=\"" + relativePath + "/" + entry->d_name + "\">" + entry->d_name + "</a></li>\n";
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
	if (waitpid(cgiProc, &cgiProcStatus, 0) == -1){
		// perror("waitpid error: ");
		// std::cout<<"1\n";
		throw(std::runtime_error("500"));
	}
	if (WIFEXITED(cgiProcStatus) && WEXITSTATUS(cgiProcStatus)){
		// std::cout<<"2\n";
		// if ({
		// 	std::cout<<"3\n";
			throw(std::runtime_error("500"));
		// }
	}

	char buf[1024];
	memset(buf,0,sizeof(buf));
	std::string cgiMessage;
	while (read(pfd[0], buf, sizeof(buf)) > 0)
		cgiMessage += buf;

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

	while (std::getline(iss, line, '\n') && line.size()){ 
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
	// std::cerr<<"pipe ok\n";
// dup
	if (dup2(pfd[1], STDOUT_FILENO) < 0)
		exit(EXIT_FAILURE);
	// std::cerr<<"dup ok\n";
//file
	const char* file = _data.at(__cgiPass).c_str();
//argv
	size_t i = _data.at(__path).find(".py");
	std::string myCgiPath;
	if (_data.at(__path).size() == i + 3 || (_data.at(__path).size() > i + 3 && _data.at(__path)[i+3] == '/'))
		myCgiPath = _data.at(__path).substr(0, i + 3);
	else
		exit(EXIT_FAILURE);
	// std::cerr<<"mycgi path: "<<myCgiPath<<"\n";
	std::vector<const char*>  argv;
	argv.push_back(file);
	argv.push_back(myCgiPath.c_str());
	argv.push_back(NULL);
	// std::cerr<<"argv ok\n";
//envp
	std::vector<const char*>  envp;
	if (_data.find(__cgiRoot) != _data.end())
		envp.push_back(const_cast<char*>(_data.at(__cgiRoot).c_str()));
	if (_data.find(__cgiMethod) != _data.end())
		envp.push_back(const_cast<char*>(_data.at(__cgiMethod).c_str()));
	if (_data.find(__cgiContentType) != _data.end())
		envp.push_back(const_cast<char*>(_data.at(__cgiContentType).c_str()));
	if (_data.find(__cgiContentLength) != _data.end())
		envp.push_back(const_cast<char*>(_data.at(__cgiContentLength).c_str()));
	if (_data.find(__cgiPath) != _data.end())
		envp.push_back(const_cast<char*>(_data.at(__cgiPath).c_str()));
	envp.push_back(nullptr);
	// std::cerr<<"envp ok\n";
//prt
	// std::cerr<<"file: "<<file<<"\n";
	// for(int i=0;argv[i]!=0;i++){
	// 	std::cerr<<"argv: "<<argv[i]<<"\n";
	// }
	// for(int i=0;envp[i]!=0;i++){
	// 	std::cerr<<"envp: "<<envp[i]<<"\n";
	// }
	if (execve(file, const_cast<char* const*>(argv.data()), const_cast<char* const*>(envp.data()))){
	//prt
		// perror("");
		std::cerr<<"failed execve: \n";
		// std::cerr<<"failed execve: \n";
		// std::cerr<<"failed execve: \n";
		exit(EXIT_FAILURE);
	}
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
	if (_data.find(__cgiRoot) != _data.end())
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