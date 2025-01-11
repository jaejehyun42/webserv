#include "Response.hpp"
#include <iostream>

using namespace std;
// make-> a.out <req>
int main(int ac, char** av){
	try{
        Request re;
        ifstream file("Request.http");
        re.initRequest(file); // 파싱, 초기화
		ServConf sc;
		sc.parse("../config/file/nginx.conf");
        Response resp(re,sc,1);
        cout<<resp.getMessage();
    }catch(exception& e){
        cerr<<e.what();
    }

}