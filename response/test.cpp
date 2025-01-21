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
#include <utility>
#include <unistd.h>
#include <unordered_map>
#include <fcntl.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
using namespace std;
enum eData{
    __statusCode,
    __reasonPhrase,
    __hostName,
    __connection,
    __path,
    __root,
    __locationIdentifier,
    __autoindex,
    __contentLength,
    __contentType,
    __requestMethod,
    __requestBody,
    __cgiPass,
    __cgiEnvData // requestCgiPath, requestQuery, requestContentLength, requestContentType, pathRoot
};
int main(){
   pair<string, string> table[19];
    cout<<sizeof(table);
    // 각 pair 객체를 생성자 사용하여 초기화
    table[0] = pair<string, string>("200", "OK");
    table[1] = pair<string, string>("201", "Created");
    table[2] = pair<string, string>("202", "Accepted");
    table[3] = pair<string, string>("203", "Non-Authoritative Information");
    table[4] = pair<string, string>("204", "No Content");
    table[5] = pair<string, string>("205", "Reset Content");
    table[6] = pair<string, string>("206", "Partial Content");
    table[7] = pair<string, string>("207", "Multi-Status");
    table[8] = pair<string, string>("208", "Already Reported");
    table[9] = pair<string, string>("226", "IM Used");
    table[10] = pair<string, string>("300", "Multiple Choices");
    table[11] = pair<string, string>("301", "Moved Permanently");
    table[12] = pair<string, string>("302", "Found");
    table[13] = pair<string, string>("303", "See Other");
    table[14] = pair<string, string>("304", "Not Modified");
    table[15] = pair<string, string>("305", "Use Proxy");
    table[16] = pair<string, string>("306", "Switch Proxy");
    table[17] = pair<string, string>("307", "Temporary Redirect");
    table[18] = pair<string, string>("308", "Permanent Redirect");

    // 출력
    for (int i = 0; i < 19; ++i) {
        cout << table[i].first << ": " << table[i].second << endl;
    }
}
// int main(){
//     unordered_map<int,string>_data;
//     string  _message;

//     int fd = open("req",O_RDONLY);
// 	char buf[1024];
// 	std::string cgiMessage;
// 	while (read(fd, buf, sizeof(buf)) > 0){
// 		cgiMessage += buf;
// 	}
// 	istringstream iss(cgiMessage);
// 	std::string line;
// 	std::string key;
// 	size_t i;
// 	std::getline(iss, line, '\n');
// 	i = line.find(' ');
// 	if (i == std::string::npos || i == 0 || i == line.size() - 1)
// 		throw std::runtime_error("500");
// 	_data[__statusCode] = line.substr(0,i);
// 	_data[__reasonPhrase] = line.substr(i+1);

// 	while (std::getline(iss, line, '\n') && line != ""){ 
// 		i = line.find(':');
// 		if (i == std::string::npos || i == 0 || i == line.size() - 1)
// 			throw std::runtime_error("500");
// 		key = line.substr(0,i);
// 		if (key == "Content-Length")
// 			_data[__contentLength] = line.substr(i+1);
// 		if (key == "Content-Type")
// 			_data[__contentType] = line.substr(i+1);
// 	}
// 	while (std::getline(iss, line, '\n') && line != "\n"){ 
// 		_message += line + "\r\n";
// 	}
//     cout<<_message;
// }

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

//cgi{
    // DIR* dir = opendir(_data.at(__path).c_str());
    // int savedErrno = errno;
    // if (dir == NULL){
    //     errno = savedErrno;
    //     throw(std::runtime_error("500"));
    // }
    // struct dirent   *entry;
    // std::string     relativePath;
    // std::string     currentPath = _data.at(__path);
    // std::string     root = _data.at(__root);
    // if (currentPath.find(root) == 0 && currentPath.size() != root.size()){
    //     relativePath = currentPath.erase(0, root.size() - 1);
    // }
    // while ((entry = readdir(dir)) != NULL){
    //     if (relativePath.size())
    //         _message += relativePath + entry->d_name;
    //     else
    //         _message += entry->d_name;
    //     _message += "\r\n"
    // }
    // _message.erase(_message.size() - 1);
    // closedir(dir);
// }

//autoindex{

// enum e{
//     __path,
//     __root
// };
// int main(int ac, char**av, char**en) {
//     string _message;
//     map<int,string> _data;
//     _data[__path] = "/Users/jeshin/42Cursus/webserv";
//     _data[__root] = "/Users/jeshin";


//     DIR* dir = opendir(_data.at(__path).c_str());
//     int savedErrno = errno;
//     if (dir == NULL){
//         errno = savedErrno;
//         throw(std::runtime_error("500"));
//     }
//     struct dirent   *entry;
//     std::string     relativePath;
//     std::string     currentPath = _data.at(__path);
//     std::string     root = _data.at(__root);
//     std::string		entryFileName;
//     if (currentPath.find(root) == 0 && currentPath.size() != root.size()){
//         relativePath = currentPath.erase(0, root.size());
//         if (currentPath.size())
//             relativePath += "/";
//     }
//     _message += 
//                 "<!DOCTYPE html>"
//                 "<html>"
//                 "<head>"
//                 "<title>Index</title>"
//                 "</head>"
//                 "<body>";
//     if (relativePath.size()){
//         _message += 
//                 "	<h1>Index of" + relativePath + "</h1>";
//     }
//                 "	<hr>"
//                 "	<ul>";

//     while ((entry = readdir(dir)) != NULL){
//         if (entry->d_name[0] == '.')
//             continue;
//         if (relativePath.size())
//             entryFileName = relativePath + entry->d_name;
//         else
//             entryFileName = entry->d_name;
//         _message +="		<li><a href=\"" + entryFileName + "\">" + entry->d_name + "</a></li>";
//     }
//     _message +=
//             "	</ul>"
//             "	<hr>"
//             "</body>"
//             "</html>";
//     closedir(dir);
//     cout<<_message;
//     return 0;
// }