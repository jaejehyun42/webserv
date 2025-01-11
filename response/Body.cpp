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

void    Body::_doGet(){ //텍스트인 경우, 바이너리인 경우 나눠야. //chunk할건지? 할거면 메모리를이용한 캐싱안될거같은데 바로send해야할거같은데 
    std::ifstream ifs(_data.at(__path), std::ios::binary);
    if (!ifs)
        throw(std::runtime_error("500"));

    ifs.seekg(0, std::ios::end); // 0:file offset
    std::streampos size = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    char* buf = new char[size];
    ifs.read(buf, size);

//파일이 바이너리인 경우  
    send(1,buf,size,0);
//텍스트인 경우
    _message += buf;
    delete[] buf;
    ifs.close();
}

void    Body::_doPost(){

}

void    Body::_doDelete(){

}