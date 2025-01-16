#include "Response.hpp"
#include "../config/ServConf.hpp"
#include <iostream>
#include <fstream>

using namespace std;

// void f(){
//     system("leaks a.out");
// }

int main(){
    // atexit(f);
    ifstream f("req");
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

        Response resp(re,conf,0);
        cout<<"-----------reponse:-----------\n";
        cout<<resp.getMessage();
    }catch(exception& e){
        cerr<<e.what();
    }
}