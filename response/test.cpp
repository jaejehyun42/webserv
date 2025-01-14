#include <sys/stat.h>
#include <iostream>
#include <errno.h>
#include <time.h>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <dirent.h>
#include <stdio.h>
#include <vector>
#include <cstdlib>
#include <unistd.h>
using namespace std;

int main(int ac, char**av, char**en) {

        char *file = const_cast<char*>("/bin/ls");
        char *argv[3] = {file, const_cast<char*>("/Users/jeshin/42Cursus/webserv"), 0}; //위험할 수 있음 수정하면 
        string s;
        for(int i=0;en[i]!=0;i++){
            s += en[i];
            s += "\n";
        }
        std::istringstream  iss(s);
        std::string         token;
        std::vector<char*>  tmpEnvp;
        while (std::getline(iss,token,' '))
            tmpEnvp.push_back(const_cast<char*>(token.c_str()));
        tmpEnvp.push_back(NULL);
        char **envp = tmpEnvp.data();

        if (execve(file, argv, envp)){
            std::cerr<<"Error: Failed execve in Cgi process\n";
            exit(EXIT_FAILURE);
        }

    return 0;
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