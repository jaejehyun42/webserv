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
    size_t  pos;
    const std::string identifier = ".py"; //.py 로 하드코딩?
    if ((pos = _data.at(__path).find(identifier)) == std::string::npos) //location block의 cgi-pass의 식별자를 알아야 함. 하드코딩안할거면. getpathiter를 써도괜찮은지?
        return ;
    int saved_errno = errno;
    if (setenv("PATH_INFO", _data.at(__path).substr(pos+identifier.size(), _data.at(__path).size()-1).c_str(), 1) == -1){
        errno = saved_errno;
        throw std::runtime_error("500");            //errno == ENOMEM, EPERM 등
    }
    if (setenv("QUERY_STRING", "_query", 1) == -1){
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