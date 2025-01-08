#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#define SERVER_IP "172.30.1.1"  // 서버 IP 주소
#define SERVER_PORT "4242"     // 서버 포트 번호

int main() {
    struct addrinfo hints, *res, *p;
    int sockfd;
    char buffer[1024];
    std::string message;

    // 힌트 초기화
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;      // IPv4 또는 IPv6 허용
    hints.ai_socktype = SOCK_STREAM;  // TCP 소켓 사용

    // getaddrinfo 호출
    int status = getaddrinfo(SERVER_IP, SERVER_PORT, &hints, &res);
    if (status != 0) {
        std::cerr << "getaddrinfo error: " << gai_strerror(status) << std::endl;
        return 1;
    }

    // 연결 가능한 주소 찾기
    for (p = res; p != nullptr; p = p->ai_next) {
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sockfd == -1) {
            perror("socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == 0) {
            break;  // 연결 성공
        }

        perror("connect");
        close(sockfd);
    }

    if (p == nullptr) {
        std::cerr << "Failed to connect to server." << std::endl;
        freeaddrinfo(res);
        return 1;
    }

    freeaddrinfo(res);  // getaddrinfo로 할당된 메모리 해제

    std::cout << "Connected to server!" << std::endl;

    // 서버와 통신
    while (true) {
        std::cout << "Enter message: ";
        std::getline(std::cin, message);

        if (message == "quit") {
            break;  // 종료
        }

        // 서버로 메시지 전송
        if (send(sockfd, message.c_str(), message.size(), 0) == -1) {
            perror("send");
            break;
        }

        // 서버로부터 응답 수신
        memset(buffer, 0, sizeof(buffer));
        int bytes_received = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0) {
            if (bytes_received == 0) {
                std::cout << "Server disconnected." << std::endl;
            } else {
                perror("recv");
            }
            break;
        }

        std::cout << "Server: " << buffer << std::endl;
    }

    // 연결 종료
    close(sockfd);
    return 0;
}
