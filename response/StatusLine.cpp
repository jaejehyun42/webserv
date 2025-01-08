#include <sys/stat.h>
#include <errno.h>
#include <unordered_map>
#include <cstring>
#include "StatusLine.hpp"
#include "../config/ServBlock.hpp"

StatusLine::StatusLine(const Request& req, const ServConf& conf, int* errCode, const int& servBlockIdx)
: _req(req), _conf(conf), _errCode(errCode), _servBlockIdx(servBlockIdx), \
_httpVersion("HTTP/1.1"), _statusCode("200"), _reasonPhrase("OK"){
    _setMessage();
}

StatusLine::~StatusLine(){}

void    StatusLine::_setPath(string& path){
    ServBlock& sb = _conf.getServBlock(_servBlockIdx);
    path = _req.getPath();
    sb.getLocation()
    path.erase(path.begin());
    sb.getRoot();
}

bool    StatusLine::_checkUrl(){
    string  path;
    struct stat urlStatusBuf;
    int saved_errno = errno;

    _setPath(path);
    if (stat(path.c_str(), &urlStatusBuf) == -1){
        if (errno == ENOENT){ //no such file and directory
            _statusCode = "404";
            _reasonPhrase = "Not Found";
        }
        else if (errno == ENOTDIR){ //not directory
            _statusCode = "404";
            _reasonPhrase = "Not Found";
        }
        else if (errno == EACCES){ //permission denied
            _statusCode = "403";
            _reasonPhrase = "Forbidden";
        }
        else{ //500 internal server error
            _statusCode = "500";
            _reasonPhrase = "Internal Server Error";
        }
        errno = saved_errno;
        *_errCode = atoi(_statusCode.c_str()); //_setErrorCode;
        _message = _httpVersion + " " + _statusCode + " " + _reasonPhrase + "\r\n";
    }
}
    
bool    StatusLine::_checkRequestError(){
    if ((_statusCode = _req.getErrorCode()).size()){
        _reasonPhrase = _req.getErrorMessage();
        *_errCode = atoi(_statusCode.c_str()); //_setErrorCode;
        _message = _httpVersion + " " + _statusCode + " " + _reasonPhrase + "\r\n";
    }
}

bool    StatusLine::_checkHeader(){
    ServBlock& sb = _conf.getServBlock(_servBlockIdx);
    unordered_map<string,string> headers = _req.getHeaders();
    
    // check host is same with server name
    if (headers["Host"] != sb.getName()){
        _statusCode = "400";
        _reasonPhrase = "Bad Request";
        *_errCode = atoi(_statusCode.c_str()); //_setErrorCode;
        _message = _httpVersion + " " + _statusCode + " " + _reasonPhrase + "\r\n";
    }
}

bool    StatusLine::_checkBody(){ 
    ServBlock& sb = _conf.getServBlock(_servBlockIdx);

    // check client_max_body_size
    if (_req.getBody().size() > sb.getMaxSize()){
        _statusCode = "413";
        _reasonPhrase = "Request Entity Too large";
        *_errCode = atoi(_statusCode.c_str()); //_setErrorCode;
        _message = _httpVersion + " " + _statusCode + " " + _reasonPhrase + "\r\n";
    }
}

void    StatusLine::_setMessage(){
    if (_checkRequestError())
        return ;
    if (_checkHeader())
        return ;
    if (_checkUrl())
        return ;
    if (_checkBody())
        return ;
    *_errCode = atoi(_statusCode.c_str()); //_setErrorCode();
    _message = _httpVersion + " " + _statusCode + " " + _reasonPhrase + "\r\n";
}

std::string  StatusLine::getMessage(){
    return (_message);
}