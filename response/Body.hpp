#ifndef __BODY_H__
#define __BODY_H__

#include <string>
using namespace std;

/* synopsis

message body:

*/

class Body{
    public:
        Body();
        ~Body();
        string  getMessage();
    private:
        string  _message;
};
#endif