#ifndef __RESPONSE_H__
#define __RESPONSE_H__


#include "StatusLine.hpp"
#include "Header.hpp"
#include "Body.hpp"
#include "Request.hpp"
#include "ServConf.hpp"

using namespace std;

/* synopsis

Response      = Status-Line                 ;
                *(( general-header          ;
                |   response-header         ;
                |   entity-header ) CRLF)   ;
                CRLF
                [ message-body ] 

*/

class Response{
    public:
        Response(int fd, Request& req, ServConf& conf);
        ~Response();
        void    response();
        void    checkRequest(); //check a request before responsing
    private:
        int                 _fd;
        const Request&      _req;
        const ServConf&     _conf;
};

#endif