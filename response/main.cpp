#include "Response.hpp"
#include "../config/ServConf.hpp"
#include <iostream>
#include <fstream>

using namespace std;

int main(int ac, char** av){
    ifstream f("Request.http");
    if (!f){
        cerr<<"Error: failed flie open\n";
        return EXIT_FAILURE;
    }
	try{
        string  s;
        string  content;
        while (getline(f,s))
            content+=s+"\r\n";
        f.close();

        Request re;
        re.initRequest(content);
        // cout<<content;

        string c = "nginx.conf";
        ServConf conf(c);

        Response resp(re,conf,1);

        cout<<resp.getMessage();
    }catch(exception& e){
        cerr<<e.what();
    }
}