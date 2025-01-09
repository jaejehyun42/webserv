#ifndef __BODY_H__
#define __BODY_H__

#include <string>
#include <unordered_map>

/* synopsis

message body:

*/

class Body{
    public:
        Body(const std::unordered_map<int, std::string>& data);
        ~Body();
        std::string  getMessage();
    private:
        std::unordered_map<int, std::string>& _data;
        std::string _message;
        void        _setMessage();
};
#endif