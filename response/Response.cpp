#include "Response.hpp"
#include "ResponseManager.hpp"

Response::Response(const Request& req, const ServConf& conf, const int& servBlockIdx){
    ResponseManager manager(req, conf, servBlockIdx);
    // _message = manager.getMessage();
    manager.printAllData();
}

Response::~Response(){}

const string&    Response::getMessage(){
    return (_message);
}