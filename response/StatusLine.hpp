#ifndef __STATUSLINE_H__
#define __STATUSLINE_H__


#include <string>
#include "Request.hpp"
#include "ServConf.hpp"
using namespace std;

/* synopsis

status line:
    Status-Line = HTTP-Version SP Status-Code SP Reason-Phrase CRLF

*/

/*
Successful : 2xx
200 : ok
201 : created
202 : accepted
203 : non-authoritative information
204 : no content
205 : reset content
206 : partial content
*/

/*
Client error : 4xx
400 : bad request
401 : unathorized
402 : payment required
403 : forbidden
404 : not found
405 : method not allowed
406 : not acceptable
407 : proxy authentication required
408 : request timeout
409 : conflict
410 : Gone
411 : length required
412 : precondition failed
413 : request entity too large
414 : request url too long
415 : unsupported media type
416 : requested range not satisfiable
417 : expectation failed
*/

/*
Server error : 5xx
500: internal server error (e.g. 자원부족, 서버의 잘못된 설정, 코드에서 처리되지 않은 예외 등)
501: not implemented
502: bad gateway
503: service unvailable
504: gateway timeout
505: HTTP version Not Supported
*/
class StatusLine{
    public:
        StatusLine(const Request& req, const ServConf& conf);
        ~StatusLine();
        string  getMessage();
        void    setMessage();
    private:
        const Request&      _req;
        const ServConf&     _conf;
        string  _message;
        string  _httpVersion;
        string  _statusCode;
        string  _reasonPhrase;
};
#endif