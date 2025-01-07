#ifndef __ENTITYHEADER_H_
#define __ENTITYHEADER_H_


#include <string>
#include "../request/Request.hpp"
#include "../config/ServConf.hpp"
using namespace std;

/* synopsis

entity header:
                   Allow                    ; Section 14.7
                      | Content-Encoding         ; Section 14.11
                      | Content-Language         ; Section 14.12
                      | Content-Length           ; Section 14.13 !
                      | Content-Location         ; Section 14.14
                      | Content-MD5              ; Section 14.15
                      | Content-Range            ; Section 14.16
                      | Content-Type             ; Section 14.17 !
                      | Expires                  ; Section 14.21
                      | Last-Modified            ; Section 14.29 !
                      | extension-header

       extension-header = message-header

*/

class EntityHeader{
    public:
        EntityHeader(const Request& req, const ServConf& conf);
        ~EntityHeader();
        string  getMessage() const;
    private:
        const Request&      _req;
        const ServConf&     _conf;
        string  _message;
};
#endif