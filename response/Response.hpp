#ifndef __RESPONSE_H__
#define __RESPONSE_H__

#include "../request/Request.hpp"
#include "../config/ServConf.hpp"

/* synopsis

Response      = Status-Line                 ;
                *(( general-header          ;
                |   response-header         ;
                |   entity-header ) CRLF)   ;
                CRLF
                [ message-body ] 

*/
// Status-Line = HTTP-Version SP Status-Code SP Reason-Phrase CRLF

class Response{
    public:
        Response(const Request& req, const ServConf& conf, const int& servBlockIdx);
        ~Response();
        string    getMessage();
    private:
        const Request&      _req;
        const ServConf&     _conf;
        const int&          _servBlockIdx;
        string              _statusCode;
        string              _message;

        bool    _checkRequestError();
        void    _setMessage();
};

#endif