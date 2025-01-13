#include <sys/stat.h>
#include <iostream>
#include <errno.h>
#include <time.h>
#include <sstream>
#include <iomanip>
#include <fstream>
using namespace std;
int main(){
    ifstream file("test");
    if (!file){
        cerr<<"Error: flie open\n";
        return EXIT_FAILURE;
    }
    string line;
    if (!getline(file, line)){
        cerr<<"Error: flie line\n";
        return EXIT_FAILURE;
    }
    while (getline(file, line)){
        cout<<line;
    }
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

    // string url = "/Users/jaeblee/ppp/kkk";
    // string location = "/";
    // string root = "/goinfre";

    // if (root.back() != '/')
    //     root.append("/");
    // size_t pos = url.find(location);
    // if (pos == 0)
    //     url.replace(pos, location.length(), root);
    // if ((pos != string::npos)){
    //     //서버루트이용.
    // }
    // cout<<url;


//     ifstream ifs("req");
//     ifs.seekg(0,ios::end);
//     streampos size = ifs.tellg();
//     ifs.seekg(0,ios::beg);
//     char* buf = new char[size];
//     ifs.read(buf, size);

// //파일이 바이너리인 경우  
//     // send(1,buf,size,0);
// //텍스트인 경우
//     string _message = "start";
//     _message += buf;
//     delete[] buf;
//     ifs.close();
//     cout<<_message<<"\n";