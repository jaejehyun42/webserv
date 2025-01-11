#ifndef __RESPONSEMANAGER_H__
#define __RESPONSEMANAGER_H__

#include "Response.hpp"
#include <string>
#include <unordered_map>

enum eData{
    __statusCode,
    __reasonPhrase,
    __hostName,
    __keepAlive,
    __path,
    __autoindex,
    __body,
    __contentType,
    __requestMethod,
    __requestPathInfo,
    __requestContentLength,
    __requestContentType,
    __requestBody
};

class ResponseManager{
    public:
        ResponseManager(const Request& req, const ServConf& conf, const int& servBlockIdx);
        ~ResponseManager();
        std::string     getMessage();
        void            printAllData();
    private:
        void        _getRequestData();
        void        _getHeaderData();

        void        _setData();
        const std::unordered_map<int, std::string>& _setErrorData(const int errCode, const string& reasonPhrase);
        void        _setMessage();
        void        _setHost();
        void        _setPath();
        void        _setRequestBody();
        void        _setErrorPath();

        void        _checkRequestError();

        const Request&      _req;
        const ServConf&     _conf;
        const ServBlock&    _sb;
        std::unordered_map<int,string>  _data;
        std::string                     _message;
};

#endif