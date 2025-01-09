#include <sys/stat.h>
#include <errno.h>
#include <unordered_map>
#include <cstring>
#include "StatusLine.hpp"
#include "../config/ServBlock.hpp"

StatusLine::StatusLine(std::unordered_map<int, std::string>& data)
: _data(data), _httpVersion("HTTP/1.1"), _statusCode("200"), _reasonPhrase("OK"){
    _setMessage();
}

StatusLine::~StatusLine(){}

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