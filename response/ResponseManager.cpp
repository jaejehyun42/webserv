#include <unordered_map>
#include <exception>
#include <sys/stat.h>
#include <errno.h>
#include <vector>
#include "ResponseManager.hpp"
#include "StatusLine.hpp"
#include "Header.hpp"
#include "Body.hpp"
#include "ErrorResponse.hpp"

ResponseManager::ResponseManager(const Request& req, const ServConf& conf, const int& servBlockIdx)
:_req(req), _conf(conf), _sb(conf.getServBlock(servBlockIdx)){
    _setMessage();
}

ResponseManager::~ResponseManager(){

}

string    ResponseManager::makeMessage(){
    return (_message);
}

void    ResponseManager::_setMessage(){
    try{
        StatusLine      statusLine(_setStatusLineData()); 
        Header          header(_setHeaderData());
        Body            body(_setBodyData());
        _message = statusLine.getMessage() + header.getMessage() + "\r\n" + body.getMessage();
    }catch(std::exception& e){
        ErrorResponse er;
        if (e.what() == "400"){
            er.setMessage(_setErrorData(400, "Bad Request"));
        }else if (e.what() == "403"){
            er.setMessage(_setErrorData(403, "Forbidden"));
        }else if (e.what() == "404"){
            er.setMessage(_setErrorData(404, "Not Found"));
        }else if (e.what() == "413"){
            er.setMessage(_setErrorData(413, "Request Entity Too Large"));
        }else if (e.what() == "500"){
            er.setMessage(_setErrorData(500, "Internal Server Error"));
        }
        _message = er.getMessage();
    }
}

const std::unordered_map<int, std::string>& ResponseManager::_setStatusLineData(){
    _checkRequestError();
    _checkHost();
    _checkPath();
    _checkBody();
    _checkContent();
}

const std::unordered_map<int, std::string>& ResponseManager::_setHeaderData(){

}

const std::unordered_map<int, std::string>& ResponseManager::_setBodyData(){
    _bodyData[__path] 
}

const std::unordered_map<int, std::string>& ResponseManager::_setErrorData(int errCode, const string& reasonPhrase){
    _errData[__statusCode] = errCode;
    _errData[__reasonPhrase] = reasonPhrase;
    _errData[__HttpVersion] = "HTTP/1.1";
    _errData[__path] = "sdf";
}

void    ResponseManager::_checkRequestError(){
    if (_req.getErrorCode().size())
        throw(std::runtime_error(_req.getErrorCode()));
}

void    ResponseManager::_checkHost(){
    unordered_map<string,string> headers = _req.getHeaders();

    if (headers.find("Host") == headers.end())
        throw std::runtime_error("400");
    std::vector<string>::const_iterator it = _sb.getName().begin();
    for (it ; it!=_sb.getName().end() ; it++){
        if (headers["Host"] == *it)
            return ;
    }
    throw std::runtime_error("400");
}

void    ResponseManager::_checkPath(){
    _setPath();
    struct stat urlStatusBuf;
    int saved_errno = errno;
    if (stat(_path.c_str(), &urlStatusBuf) == -1){
        if (errno == ENOENT)                //no such file and directory
            throw std::runtime_error("404");
        else if (errno == ENOTDIR)          //not directory
            throw std::runtime_error("404");
        else if (errno == EACCES)           //permission denied
            throw std::runtime_error("403");
        else                                //500 internal server error
            throw std::runtime_error("500");
        errno = saved_errno;
    }
    _statusLineData[__path] = _path;
}

void    ResponseManager::_checkBody(){
    if (_req.getBody().size())
        return ;
    if (_sb.getMaxSize())
        throw (std::runtime_error("413"));
    _statusLineData[__body] = _req.getBody();
}

void    ResponseManager::_setPath(){
    _path = _req.getPath();
    
    ServBlock sb; //sb를 _sb로 변경해야함. 
    std::unordered_map<string, LocBlock>::iterator it =  sb.getPathIter(_path);
    std::string locationIdentifier = it->first;
    LocBlock lb = it->second;
    std::string root = lb.getRoot();

    if (root.empty()){
        root = sb.getRoot(); 
        _path += root;
    }
    size_t pos = _path.find(root);
    if (pos == 0){
        _path.replace(pos, locationIdentifier.length(), root);
    }
    else(pos != string::npos){
        root = sb.getRoot();
        _path += root;
    }
}

void    ResponseManager::_setErrPath(){
    _sb.getErrorPage()   
}
