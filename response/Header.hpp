#ifndef __HEADER_H_
#define __HEADER_H_

#include <string>
#include "Request.hpp"
#include "ServConf.hpp"
#include "GeneralHeader.hpp"
#include "ResponseHeader.hpp"
#include "EntityHeader.hpp"
using namespace std;

/* synopsis

header : 
                (( general-header          ;
                |   response-header         ;
                |   entity-header ) CRLF)   ;

*/

class Header{
    public:
        Header(const Request& req, const ServConf& conf);
        ~Header();
        string  getMessage();
    protected:
        const   Request&    _req;
        const   ServConf&   _conf;
        string              _message;
};
#endif