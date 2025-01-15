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
	waitpid(cgiProc, &cgiProcStatus,0);
	if (WEXITSTATUS(cgiProcStatus)){
		std::cout<<"******cgiProcStatus**********";
		throw(std::runtime_error("501"));//500
	}

	char buf[1024];
	std::string cgiMessage;
	std::cout<<"********start********";
	while (read(pfd[0], buf, sizeof(buf)) > 0){
		cgiMessage += buf;
		std::cout<<buf;
	}
	std::cout<<"********end8********";
	istringstream iss(cgiMessage);
	std::string line;
	std::string key;
	size_t i;
	std::getline(iss, line, '\n');
	i = line.find(' ');
	if (i == std::string::npos || i == 0 || i == line.size() - 1){
		std::cout<<"******iiiiiiiiiiiiii**********";
		throw std::runtime_error("502"); //500
	}
	_data[__statusCode] = line.substr(0,i);
	_data[__reasonPhrase] = line.substr(i+1);

	while (std::getline(iss, line, '\n') && line != ""){ 
		i = line.find(':');
		if (i == std::string::npos || i == 0 || i == line.size() - 1)
			throw std::runtime_error("503"); //500
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

	if (dup2(pfd[1], STDOUT_FILENO) < 0){

		std::cerr<<"******dup**********";
		exit(EXIT_FAILURE);
	}

	char *file = const_cast<char*>(_data.at(__cgiPass).c_str());
	char *argv[3] = {file, const_cast<char*>("/Users/jeshin/goinfre/my_cgi.py"), 0};

	// if (_data.find(__cgiEnvData) == _data.end()){
	// 	std::cerr<<"******cgiEndData**********";
	// 	exit(EXIT_FAILURE);
	// }
	const char* env_array[] = {
        "PATH_INFO=/seung.txt",
        "QUERY_STRING=",
        "REQUESTED_METHOD=GET",
        "CONTENT_LENGTH=0",
        "CONTENT_TYPE=text/plain",
        "DOCUMENT_ROOT=/Users/jeshin/goinfre/seunghan",
        nullptr // 마지막에 nullptr 추가
    };

    // char** envp에 할당
    char** envp = new char*[sizeof(env_array) / sizeof(env_array[0])];
    for (size_t i = 0; env_array[i] != nullptr; ++i) {
        envp[i] = new char[std::strlen(env_array[i]) + 1];
        std::strcpy(envp[i], env_array[i]);
    }
    envp[sizeof(env_array) / sizeof(env_array[0]) - 1] = nullptr;
	std::cerr<<"*******file: "<<file<<"\n";
	for(int i=0; argv[i]!=0;i++){
		std::cerr<<"*******argv: "<<argv[i]<<"\n";
	}
	for(int i=0; envp[i]!=0;i++){
		std::cerr<<"*******envp: "<<envp[i]<<"\n";
	}
	// vector<char*> temp;
	// temp.push_back("PATH_INFO = /seung.txt");
	// temp.push_back("QUERY_STRING=");
	// temp.push_back("REQUESTED_METHOD=GET");
	// temp.push_back("CONTENT_LENGTH = 0");
	// temp.push_back("CONTENT_TYPE = text/plain");
	// temp.push_back("DOCUMENT_ROOT = /Users/jeshin/goinfre/seunghan");
	char *args[] = {"/bin/ls", NULL};
	// char **envp = temp.data();
	if (execve(*args, args, envp) == -1){
		perror("");
		std::cerr<<")))))))))))))))))))))))))))))00";
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