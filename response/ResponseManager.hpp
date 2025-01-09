#ifndef __RESPONSEMANAGER_H__
#define __RESPONSEMANAGER_H__

#include "Response.hpp"
#include <string>

enum Data{
    __statusCode,
    __reasonPhrase,
    __HttpVersion,
    __path,
    __pathInfo,
    __contentLength,
    __contentType,
    __query,
    __body,
};

class ResponseManager{
    public:
        ResponseManager(const Request& req, const ServConf& conf, const int& servBlockIdx);
        ~ResponseManager();
        std::string    makeMessage();
    private:
        void        _setMessage();
        void        _setPath();
        void        _setErrPath();
        void        _checkRequestError();
        void        _checkHost();
        void        _checkBody();
        void        _checkPath();

        const std::unordered_map<int, std::string>& _setStatusLineData();
        const std::unordered_map<int, std::string>& _setHeaderData();
        const std::unordered_map<int, std::string>& _setBodyData();
        const std::unordered_map<int, std::string>& _setErrorData(const int errCode, const string& reasonPhrase);

        const Request&      _req;
        const ServConf&     _conf;
        const ServBlock&    _sb;    //server block
        std::unordered_map<int,string>  _data;
        std::unordered_map<int,string>  _errData;
        std::string                     _message;
        std::string                     _path;
};

#endif