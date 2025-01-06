#ifndef __HEADER_H_
#define __HEADER_H_

#include <string>
#include "GeneralHeader.hpp"
#include "ResponseHeader.hpp"
#include "EntityHeader.hpp"
#include "ServConf.hpp"
#include "Request.hpp"
using namespace std;

/* synopsis

header : 
                (( general-header          ;
                |   response-header         ;
                |   entity-header ) CRLF)   ;

*/

class Header{
    public:
        Header();
        // Header(ServConf conf, Request req);
        ~Header();
        string  getMessage();
    private:
        // const   ServConf&   _conf;
        // const   Request&    _req;
        string              _message;
};
#endif