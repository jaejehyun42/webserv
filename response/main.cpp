#include "Response.hpp"
#include <iostream>
#include <fstream>

using namespace std;

int main(int ac, char** av){
	try{
        Request re;
        ifstream file("test");
        if (!file.is_open()){
            cerr<<"Error: failed flie open\n";
            return EXIT_FAILURE;
        }
        re.initRequest("Request.http"); // 파싱, 초기화
        ServConf sc;
		sc.parse("../config/file/nginx.conf");
        Response resp(re,sc,1);
        cout<<resp.getMessage();
    }catch(exception& e){
        cerr<<e.what();
    }

}