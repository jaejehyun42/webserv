#include <sys/stat.h>
#include <errno.h>
#include <unordered_map>
#include "StatusLine.hpp"

StatusLine::StatusLine(const Request& req, const ServConf& conf)
: _req(req), _conf(conf), _httpVersion("HTTP/1.1"), _statusCode("200"), _reasonPhrase("ok"){
    _setMessage();
}

StatusLine::~StatusLine(){}

bool    StatusLine::_checkUrl(){
    struct stat urlStatusBuf;
    int saved_errno = errno;

    if (stat(_req.getPath().c_str(), &urlStatusBuf) == -1){
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
        _message = _httpVersion + " " + _statusCode + " " + _reasonPhrase + "\r\n";
        return (EXIT_FAILURE);
    }
    return (EXIT_SUCCESS);
}
    
bool    StatusLine::_checkRequestError(){
    if ((_statusCode = _req.getErrorCode()).size()){
        _reasonPhrase = _req.getErrorMessage();
        _message = _httpVersion + " " + _statusCode + " " + _reasonPhrase + "\r\n";
        return (EXIT_FAILURE);
    }
    return (EXIT_SUCCESS);
}

bool    StatusLine::_checkHeader(){
    //Host가 요청 path와 다른지 
    //클라이언트 body 길이 체크 
    return (EXIT_SUCCESS);
}

void    StatusLine::_setMessage(){
    if (_checkRequestError())
        return ;
    if (_checkUrl())
        return ;
    if (_checkHeader())
        return ;
    _message = _httpVersion + " " + _statusCode + " " + _reasonPhrase + "\r\n";
}

std::string  StatusLine::getMessage(){
    return (_message);
}

