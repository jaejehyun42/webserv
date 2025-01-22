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
#include <time.h>
#include <signal.h>
#include "Body.hpp"
#include "ResponseManager.hpp"

Body::Body(std::map<int, std::string>& data): _data(data){
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

void    Body::_processCgiMessage(pid_t& cgiProc, int* cgiReadFd, int* cgiWriteFd){
	close(cgiReadFd[0]);
	close(cgiWriteFd[1]);
	if (_data.find(__requestBody) != _data.end()){
		if (write(cgiReadFd[1], _data.at(__requestBody).c_str(), _data.at(__requestBody).size() + 1) == -1){
			close(cgiReadFd[1]);
			throw(std::runtime_error("500"));
		}
	}
	close(cgiReadFd[1]);

	int cgiProcStatus;
    time_t start = time(NULL);
    while (1) {
        pid_t result = waitpid(cgiProc, &cgiProcStatus, WNOHANG);
        if (result > 0) { // 자식 프로세스 종료
			break;
        } else if (result == 0) { // 자식 프로세스가 아직 종료되지 않음
            if (time(NULL) - start >= 5) { // 시간 초과
				kill(cgiProc, SIGKILL);
				throw std::runtime_error("500");
            }
            usleep(100); // 100ms 대기
        } else {
			throw std::runtime_error("500");
        }
    }
	if (WEXITSTATUS(cgiProcStatus))
		throw(std::runtime_error("500"));

	char buf[1024];
	ssize_t bufReadSize;
	std::string cgiMessage;
	while ((bufReadSize = read(cgiWriteFd[0], buf, sizeof(buf) - 1)) > 0){
		buf[bufReadSize] = 0;
		cgiMessage.append(buf);
	}
	close(cgiWriteFd[0]);

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

//코드값이 세자리수가아니거나 앞자리가 4또는 5인경우
	if (_data[__statusCode].size() != 3 || \
	(_data[__statusCode][0] == '4' || _data[__statusCode][0] == '5')){
		throw(std::runtime_error(_data[__statusCode]));
	}
	
	while (std::getline(iss, line, '\n') && line.size()){ 
		i = line.find(' ');
		if (i == std::string::npos || i == 0 || i == line.size() - 1)
			throw std::runtime_error("500");
		key = line.substr(0,i);
		if (key == "Content-Length:")
			_data[__contentLength] = line.substr(i+1);
		if (key == "Content-Type:")
			_data[__contentType] = line.substr(i+1);
	}
	while (std::getline(iss, line, '\n') && line != "\n"){ 
		_message += line + "\r\n";
	}
}

void	Body::_execCgiProc(int* cgiReadFd, int* cgiWriteFd){
	close(cgiReadFd[1]);
	close(cgiWriteFd[0]);
// dup
	if (dup2(cgiReadFd[0], STDIN_FILENO) < 0)
		exit(EXIT_FAILURE);
	close(cgiReadFd[0]);
	if (dup2(cgiWriteFd[1], STDOUT_FILENO) < 0)
		exit(EXIT_FAILURE);
	close(cgiWriteFd[1]);
//file
	const char* file = _data.at(__cgiPass).c_str();
	// std::cerr<<"file: "<<file<<"\n";
//argv
	size_t i = _data.at(__path).find(".py");
	std::string myCgiPath;
	if (_data.at(__path).size() == i + 3 || (_data.at(__path).size() > i + 3 && _data.at(__path)[i+3] == '/'))
		myCgiPath = _data.at(__path).substr(0, i + 3);
	else
		exit(EXIT_FAILURE);
	std::vector<const char*>  argv;
	argv.push_back(file);
	argv.push_back(myCgiPath.c_str());
	argv.push_back(NULL);
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
	if (_data.find(__cgiQuery) != _data.end())
		envp.push_back(const_cast<char*>(_data.at(__cgiQuery).c_str()));
	envp.push_back(nullptr);
//prt
	// for(int i=0;argv[i]!=0;i++)
	// 	std::cerr<<"argv: "<<argv[i]<<"\n";
	// for(int i=0;envp[i]!=0;i++)
	// 	std::cerr<<"envp: "<<envp[i]<<"\n";
	if (execve(file, const_cast<char* const*>(argv.data()), const_cast<char* const*>(envp.data()))){
		perror("failed execve");
		exit(EXIT_FAILURE);
	}
}

void    Body::_makeCgiMessage(){
	int cgiReadFd[2];
	int cgiWriteFd[2];
	int savedErrno;
	savedErrno = errno;

	if (pipe(cgiReadFd) < 0){
		errno = savedErrno;
		throw(std::runtime_error("500"));
	}
	if (pipe(cgiWriteFd) < 0){
		errno = savedErrno;
		throw(std::runtime_error("500"));
	}
	pid_t cgiProc = fork();
	if (cgiProc < 0){
		errno = savedErrno;
		throw(std::runtime_error("500"));
	}
	cgiProc == 0 ? _execCgiProc(cgiReadFd, cgiWriteFd) : _processCgiMessage(cgiProc, cgiReadFd, cgiWriteFd);
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