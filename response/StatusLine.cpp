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

void    StatusLine::_setCgiEnv(){
    if (_data.at(__pathInfo).empty())
        return ;

    int saved_errno;
    saved_errno = errno;
    if (setenv("PATH_INFO", _data.at(__pathInfo).c_str(), 1) == -1){
        errno = saved_errno;
        throw std::runtime_error("500");            //errno == ENOMEM 등
    }
    if (setenv("QUERY_STRING", "_query", 1) == -1){
        errno = saved_errno;
        throw std::runtime_error("500");
    }
    if (_data.at(__contentLength).size() && \
    (setenv("CONTENT_LENGTH", _data.at(__contentLength).c_str(), 1) == -1)){
        errno = saved_errno;
        throw std::runtime_error("500");
    }
    if (_data.at(__contentType).size() && \
    setenv("CONTENT_TYPE", _data.at(__contentType).c_str(), 1) == -1){
        errno = saved_errno;
        throw std::runtime_error("500");
    }
}

void    StatusLine::_setMessage(){
    if (_checkHeader())
        return ;
    if (_checkUrl())
        return ;
    _message = _httpVersion + " " + _statusCode + " " + _reasonPhrase + "\r\n";
}

std::string  StatusLine::getMessage(){
    return (_message);
}