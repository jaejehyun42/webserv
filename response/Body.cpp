#include <string>
#include <iostream>
#include <fstream>
#include <sys/socket.h>
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
    if (_data.at(__requestMethod) == "GET")
        _doGet();
    else if (_data.at(__requestMethod) == "POST")
        _doPost();
    else if (_data.at(__requestMethod) == "DELETE")
        _doDelete();
}

void    Body::_doGet(){
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

void    Body::_doPost(){

}

void    Body::_doDelete(){

}