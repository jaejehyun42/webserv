#include <errno.h>
#include <unordered_map>
#include "StatusLine.hpp"
#include "ResponseManager.hpp"

StatusLine::StatusLine(const std::unordered_map<int, std::string>& data)
: _data(data), _httpVersion("HTTP/1.1"), _statusCode("200"), _reasonPhrase("OK"){
    _setMessage();
}

StatusLine::~StatusLine(){

}

std::string  StatusLine::getMessage(){
    return (_message);
}

void    StatusLine::_setMessage(){
    _message = _httpVersion + " " + _statusCode + " " + _reasonPhrase + "\r\n";
}

void    StatusLine::_setCgiEnv(){
    int saved_errno = errno;

    if (setenv("REQUESTED_METHOD", _data.at(__requestMethod).c_str(), 1) == -1){
        errno = saved_errno;
        throw std::runtime_error("500");
    }
    if (_data.at(__requestCgiPath).size() && \
    (setenv("PATH_INFO", _data.at(__requestCgiPath).c_str(), 1) == -1)){
        errno = saved_errno;
        throw std::runtime_error("500");            //errno == ENOMEM, EPERM 등
    }
    if (_data.at(__requestQuery).size() && \
    (setenv("QUERY_STRING", _data.at(__requestQuery).c_str(), 1) == -1)){
        errno = saved_errno;
        throw std::runtime_error("500");
    }
    if (_data.at(__requestContentLength).size() && \
    (setenv("CONTENT_LENGTH", _data.at(__requestContentLength).c_str(), 1) == -1)){
        errno = saved_errno;
        throw std::runtime_error("500");
    }
    if (_data.at(__contentType).size() && \
    setenv("CONTENT_TYPE", _data.at(__requestContentType).c_str(), 1) == -1){
        errno = saved_errno;
        throw std::runtime_error("500");
    }
}