#include "Response.hpp"
#include <iostream>

using namespace std;
// make-> a.out <req>
int main(int ac, char** av){
    if (ac != 2)
        return (1);

    ifstream ifs(av[1]);
    if (!ifs)
        return (1);

    ServConf conf;
	Request re;

	try{
        re.initRequest(ifs);
    }catch(exception& e){
        Response resp(re, conf);
        resp.response(1);
    }

}