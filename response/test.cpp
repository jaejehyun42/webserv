#include <sys/stat.h>
#include <iostream>
#include <errno.h>
#include <time.h>
#include <sstream>
#include <iomanip>
using namespace std;
int main(){

//Date: Tue, 07 Jan 2025 05:29:17 GMT

    std::time_t         now = std::time(NULL);
    struct tm*          gmt = gmtime(&now); //GMT 시간버퍼
    std::ostringstream  dateStream;
    dateStream << std::put_time(gmt, "%a, %d %b %Y %H:%M:%S GMT");
    std::cout << dateStream.str();





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