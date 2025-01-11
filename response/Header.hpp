#ifndef __HEADER_H_
#define __HEADER_H_

#include <string>
#include <unordered_map>

/* synopsis

header : 
                (( general-header          ;
                |   response-header         ;
                |   entity-header ) CRLF)   ;

*/

class Header{
    public:
        Header(const std::unordered_map<int, std::string>& data);
        ~Header();
        std::string  getMessage();
    private:
        const std::unordered_map<int, std::string>& _data;
        std::string              _message;
        void                _setMessage();
};
#endif