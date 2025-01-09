#ifndef __RESPONSEMANAGER_H__
#define __RESPONSEMANAGER_H__

#include "Response.hpp"
#include <string>

class ResponseManager{
    public:
        ResponseManager(const Request& req, const ServConf& conf, const int& servBlockIdx);
        ~ResponseManager();
        std::string    response();
    private:
        void        _setMessage();

        void        _checkRequest();
        void        _checkHost();
        void        _checkBody();
        void        _checkPath();

        string      _getPath();
        std::unordered_map<int, std::string>& _getStatusLineDate();
        std::unordered_map<int, std::string>& _getHeaderDate();
        std::unordered_map<int, std::string>& _getBodyDate();

        const Request&      _req;
        const ServConf&     _conf;
        const ServBlock&    _sb;
        std::unordered_map<int,string> _statusLineData;
        std::string         _message;
};

#endif