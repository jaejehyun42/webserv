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

class Response{
    public:
        Response(Request& req, ServConf& conf);
        ~Response();
        void    response(int fd);
        void    checkRequest(); //check a request before responsing
    private:
        const Request&      _req;
        const ServConf&     _conf;
};

#endif