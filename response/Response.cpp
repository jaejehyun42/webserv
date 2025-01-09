#include "Response.hpp"
#include "ResponseManager.hpp"

Response::Response(const Request& req, const ServConf& conf, const int& servBlockIdx){
    ResponseManager manager(req, conf, servBlockIdx);
    _message = manager.response();
}

Response::~Response(){

}

string    Response::getMessage(){
    return (_message);
}