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

Body::Body(const std::unordered_map<int, std::string>& data): _data(data){
    _setMessage();
}

Body::~Body(){}

std::string  Body::getMessage(){
    return (_message);
}
void        Body::_setMessage(){
    if (_data.find(__requestMethod) == _data.end())
        throw std::runtime_error("500");
    if (_data.at(__requestMethod) == "GET")
        _makeGetMessage();
    else if (_data.at(__requestMethod) == "POST")
        _makePostMessage();
    else if (_data.at(__requestMethod) == "DELETE")
        _makeDeleteMessage();
}

void    Body::_makeAutoindexMessage(){
    DIR* dir = opendir(_data.at(__path).c_str());
    int savedErrno = errno;
    if (dir == NULL){
        throw(std::runtime_error("500"));
        errno = savedErrno;
    }
    struct dirent   *entry;
    while ((entry = readdir(dir)) != NULL){
        _message += entry->d_name;
        _message += "\r\n";
    }
    closedir(dir);
}

void    Body::_makeCgiMessage(){
    int pfd[2];
    int savedErrno;
    savedErrno = errno;

    if (pipe(pfd) < 0){
        std::cerr<<"Error: Failed pipe\n";
        errno = savedErrno;
        throw(std::runtime_error("500"));
    }
    pid_t cgiProc = fork();
    if (cgiProc < 0){
        std::cerr<<"Error: failed fork cgi process\n";
        errno = savedErrno;
        throw(std::runtime_error("500"));
    }else if (cgiProc == 0){ //자식프로세스
        close(pfd[0]);
        if (dup2(pfd[1], STDOUT_FILENO) < 0){
            std::cerr<<"Error: Failed dup2 Cgi process\n";
            exit(EXIT_FAILURE);
        }

        char *file = const_cast<char*>(_data.at(__cgiPass).c_str());
        char *argv[3] = {file, const_cast<char*>("my_cgi.py"), 0}; //위험할 수 있음 수정하면 

        std::istringstream  iss(_data.at(__cgiEnvData));
        std::string         token;
        std::vector<char*>  tmpEnvp;
        while (std::getline(iss,token,' '))
            tmpEnvp.push_back(const_cast<char*>(token.c_str()));
        tmpEnvp.push_back(NULL);
        char **envp = tmpEnvp.data();

        if (execve(file, argv, envp)){
            std::cerr<<"Error: Failed execve in Cgi process\n";
            exit(EXIT_FAILURE);
        }
        exit(EXIT_SUCCESS);
    }else{ //부모프로세스
        close(pfd[1]);
        int cgiProcStatus;
        waitpid(cgiProc, &cgiProcStatus,0);
        if (WEXITSTATUS(cgiProcStatus)){
            throw(std::runtime_error("500"));
        }
        char buf[1024];
        while (read(pfd[0],buf,sizeof(buf)) > 0){
            _message += buf;
        }
    }
}

void    Body::_makeGetMessage(){
    if (_data.find(__cgiEnvData) != _data.end()){
        _makeCgiMessage();
        return ;
    }
    if (_data.find(__autoindex) != _data.end()){
        _makeAutoindexMessage();
        return ;
    }
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

void    Body::_makePostMessage(){

}

void    Body::_makeDeleteMessage(){

}