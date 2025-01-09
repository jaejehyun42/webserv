#include <sys/stat.h>
#include <iostream>
#include <errno.h>
#include <time.h>
#include <sstream>
#include <iomanip>
using namespace std;
int main(){
    string url = "/Users/jaeblee/ppp/kkk";
    string location = "/";
    string root = "/goinfre";

    if (root.back() != '/')
        root.append("/");
    size_t pos = url.find(location);
    if (pos == 0)
        url.replace(pos, location.length(), root);
    if ((pos != string::npos)){
        //서버루트이용.
    }
    cout<<url;
}
    // struct stat urlStatusBuf;

    // if (stat("/Users/jeshin/42Cursus/webserv/dockerfile/hihi", &urlStatusBuf) == -1){
    //     if (errno ==  ENOENT){
    //         cout<<"ENOENT\n";
    //         // cout<<errno;
    //         return 1;
    //     }
    //     if (errno == ENOTDIR){//501 internal server error
    //         cout<<"ENOTDIR\n";
    //         // cout<<errno;
    //         return 1;
    //     }
    // }
    // cout<<"ok";