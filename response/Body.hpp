#ifndef __BODY_H__
#define __BODY_H__

#include <string>
#include "../request/Request.hpp"
#include "../config/ServConf.hpp"
using namespace std;

/* synopsis

message body:

*/

class Body{
    public:
        Body(const Request& req, const ServConf& conf);
        ~Body();
        string  getMessage() const;
    private:
        const Request&      _req;
        const ServConf&     _conf;
        string  _message;
};
#endif