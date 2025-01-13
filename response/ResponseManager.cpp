#include <sys/stat.h>
#include <errno.h>
#include <exception>
#include <unordered_map>
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

ResponseManager::~ResponseManager(){}

string    ResponseManager::getMessage(){
    return (_message);
}

void    ResponseManager::_setHeaderData(){
//keep-alive 
    ostringstream oss;
    if (oss <<_conf.getAliveTime())
        _data[__keepAlive] = oss.str();
//content-type
    size_t i = _data[__path].find_last_of('.');
    if (i == std::string::npos || (i + 1 == _data[__path].size()))
        _data[__contentType] = _conf.getMime("default");
    else
        _data[__contentType] = _conf.getMime(_data[__path].substr(i));
}

void    ResponseManager::_setMessage(){
    try{
        _setData();
        StatusLine      statusLine(_data); 
        Header          header(_data);
        Body            body(_data);
        _message = statusLine.getMessage() + header.getMessage() + "\r\n" + body.getMessage();
    }catch(std::exception& e){
        ErrorResponse er;
        if (std::string(e.what()) == "400"){
            er.setMessage(_setErrorData(400, "Bad Request"));
        }else if (std::string(e.what()) == "403"){
            er.setMessage(_setErrorData(403, "Forbidden"));
        }else if (std::string(e.what()) == "404"){
            er.setMessage(_setErrorData(404, "Not Found"));
        }else if (std::string(e.what()) == "405"){
            er.setMessage(_setErrorData(405, "Method Not Allowed"));
        }else if (std::string(e.what()) == "413"){
            er.setMessage(_setErrorData(413, "Request Entity Too Large"));
        }else if (std::string(e.what()) == "500"){
            er.setMessage(_setErrorData(500, "Internal Server Error"));
        }else if (std::string(e.what()) == "505"){
            er.setMessage(_setErrorData(505, "HTTP Version Not Supported"));
        }else{
            er.setMessage(_setErrorData(500, "Internal Server Error"));
        }
        _message = er.getMessage();
    }
}

void    ResponseManager::_setData(){
    _checkRequestError();
    _setHost();
    _setPath();
    _setRequestBody();
    _setRequestCgiEnv();
    _setHeaderData();
}

const std::unordered_map<int, std::string>& ResponseManager::_setErrorData(int errCode, const string& reasonPhrase){
    _data[__statusCode] = errCode;
    _data[__reasonPhrase] = reasonPhrase;
    const std::string& errorPage = _sb.getErrorPage(strtol(_data[__statusCode].c_str(), 0, 10));
    if (errorPage.size())
        _data[__path] = _sb.getRoot() + "/" + errorPage;
    else
        _data[__path] = ""; 
    return (_data);
}

void    ResponseManager::_setHost(){
    unordered_map<string,string> headers = _req.getHeaders();
    if (headers.find("Host") == headers.end())
        throw std::runtime_error("400");

    std::vector<string>::const_iterator it = _sb.getName().begin();
    for (; it!=_sb.getName().end() ; it++){
        if (headers["Host"] == *it){
            _data[__hostName] = *it;
            return ;
        }
    }
    throw std::runtime_error("400");
}

void    ResponseManager::_setPath(){
    string      path = _req.getPath();
    struct stat pathStatus;
    int         saved_errno = errno;
    std::unordered_map<string, LocBlock>::const_iterator it =  _sb.getPathIter(path);
    const std::string&  serverRoot = _sb.getRoot();
    const std::string&  locationIdentifier = it->first;
    const LocBlock&     locationBlock = it->second;
    const std::string&  locationRoot = locationBlock.getRoot();
    
//이름이 어떤 로케이션블록 또는 서버블록의 루트와 완전히 일치하는게 있는지 있다면 그것을 사용하는데 반영?
// 근데 로케이션 블록이 / 이라 ""로 되는지 아니면 아예 로케이션 블록이 없어서 ""인지 어떻게 아는지?
    if (it == _sb.getPath().end() && serverRoot.size())//매핑되는 로케이션 블록이 없을 경우. 서버 루트 사용.
        path.insert(0,serverRoot);
    else if (locationRoot.empty())//매핑되는 로케이션블록이 있지만 로케이션블록의 루트가 없는경우. 서버 루트 이용.
        path.insert(0,_sb.getRoot());
    else
        path.replace(0, locationIdentifier.size()-1, locationRoot);//매핑되는 로케이션 블록이 있고 루트도 있는 경우. 로케이션 블록의 루트 사용

    if (stat(path.c_str(), &pathStatus) == -1){
        if (errno == ENOENT && (errno = saved_errno))//no such file and directory
            throw std::runtime_error("404");
        else if (errno == ENOTDIR && (errno = saved_errno))//not directory
            throw std::runtime_error("404");
        else if (errno == EACCES && (errno = saved_errno))//permission denied          
            throw std::runtime_error("403");
        else{                               
            errno = saved_errno;
            throw std::runtime_error("500");//500 internal server error
        }
    }
    _data[__path] = path;
    if (S_ISDIR(pathStatus.st_mode)){
        const std::vector<string> indexs = locationBlock.getIndex();
        std::string savedPath = path;
        saved_errno = errno;
        if (indexs.size()){
            for(size_t i=0; i<indexs.size(); i++){
                path = savedPath;
                if (*(path.end() - 1) == '/')
                    path += indexs[i];
                else
                    path += "/" + indexs[i];
                if (stat(path.c_str(), &pathStatus) == EXIT_SUCCESS){
                    _data[__path] = path;
                    errno = saved_errno;
                    return ;
                }
            }
            errno = saved_errno;
        }
        if(locationBlock.getAutoindex()){
            _data[__path] = "AUTOINDEX";
            return ;
        }
        throw std::runtime_error("404");
    }
}

void    ResponseManager::_setRequestBody(){
    std::string requestBody = _req.getBody();
    if (requestBody.empty())
        return ;
    if (requestBody.size() < _sb.getMaxSize())
        throw (std::runtime_error("413"));
    _data[__requestBody] = requestBody;
}

void ResponseManager::_setRequestCgiEnv(){
    if (_req.getCgiPath().size())
        _data[__requestPathInfo] = _req.getCgiPath();
    if (_req.getQuery().size())
        _data[__requestQuery] = _req.getQuery();
}

void    ResponseManager::_checkRequestError(){
    if (_req.getErrorCode().size())
        throw(std::runtime_error(_req.getErrorCode()));
}

void    ResponseManager::printAllData(){
    if (_data.find(__statusCode)!=_data.end())
        cout<<_data[__statusCode];
    if (_data.find(__reasonPhrase)!=_data.end())
        cout<<_data[__reasonPhrase];
    if (_data.find(__hostName)!=_data.end())
        cout<<_data[__hostName];
    if (_data.find(__keepAlive)!=_data.end())
        cout<<_data[__keepAlive];
    if (_data.find(__path)!=_data.end())
        cout<<_data[__path];
    if (_data.find(__autoindex)!=_data.end())
        cout<<_data[__autoindex];
    if (_data.find(__body)!=_data.end())
        cout<<_data[__body];
    if (_data.find(__contentType)!=_data.end())
        cout<<_data[__contentType];
    if (_data.find(__requestMethod)!=_data.end())
        cout<<_data[__requestMethod];
    if (_data.find(__requestPathInfo)!=_data.end())
        cout<<_data[__requestPathInfo];
    if (_data.find(__requestContentLength)!=_data.end())
        cout<<_data[__requestContentLength];
    if (_data.find(__requestContentType)!=_data.end())
        cout<<_data[__requestContentType];
    if (_data.find(__requestBody)!=_data.end())
        cout<<_data[__requestBody];
}