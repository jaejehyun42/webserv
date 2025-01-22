#include "Header.hpp"
#include "GeneralHeader.hpp"
#include "ResponseHeader.hpp"
#include "EntityHeader.hpp"

Header::Header(const std::map<int, std::string>& data) : _data(data){
    _setMessage();
}

Header::~Header(){}

string  Header::getMessage(){
    return (_message);
}

void    Header::_setMessage(){
    GeneralHeader   generalHeader(_data);
    ResponseHeader  responseHeader(_data);
    EntityHeader    entityHeader(_data);

    _message = generalHeader.getMessage() + responseHeader.getMessage() + entityHeader.getMessage() + "\r\n";
}
