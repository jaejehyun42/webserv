#include <string>
#include <unordered_map>
#include "ErrorResponse.hpp"

ErrorResponse::ErrorResponse(){}

ErrorResponse::~ErrorResponse(){}

void            ErrorResponse::setMessage(const std::unordered_map<int, std::string>& errData){
    message = errData.at(__HttpVersion) + " " + errData.at(__statusCode) + " " + errData.at(__reasonPhrase) + "\r\n";
}

const std::string    ErrorResponse::getMessage(){
    return (message);
}

