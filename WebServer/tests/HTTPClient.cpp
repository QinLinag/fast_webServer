#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
#include <iostream>

#define MACXSIZE 1024
#define IPADDRESS "127.0.0.1"
#define SERV_PORT 6598
#define FDSIZE 1024
#define EPOLLEVENTS 20

int setSocketNonBlocking(int fd) {
    int flag = fcntl(fd, F_GETFL, 0);
    if(flag == -1) {
        return -1;
    }
    flag |= O_NONBLOCK;
    if(fcntl(fd, F_SETFL, flag) == -1) {
        return -1;
    }
    return 0;
}



int main(int argc, char** argv) {
    int sockfd;
    struct sockaddr_in servaddr;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET, IPADDRESS, &servaddr.sin_addr);
    char buff[4096];
    buff[0] = '\0';

    const char* p = " ";
    if(connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == 0) {
        setSocketNonBlocking(sockfd);
        std::cout << "1:" << std::endl;
        ssize_t n = write(sockfd, p, strlen(p));
        std::cout << "strlen(p)= " << strlen(p) << std::endl;
        sleep(1);
        n = read(sockfd, buff, 4096);
        std::cout << "n=" << n << std::endl;
        printf("%s", buff);
        close(sockfd);
    } else {
        perror("err1");
    }
    sleep(1);

    //发”GET HTTP/1.1“
    p = "GET HTTP/1.1";
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == 0) {
        setSocketNonBlocking(sockfd);
        std::cout << "2:" << std::endl;
        ssize_t n = write(sockfd, p, strlen(p));
        std::cout << "strlen(p)= " << strlen(p) << std::endl;
        sleep(1);
        n = read(sockfd, buff, 4096);
        std::cout << "n=" << n << std::endl;
        printf("%s", buff);
        close(sockfd);
    } else {
        perror("err2");
    }
    sleep(1);

    //发
    //GET HTTP/1.1
    //HOST： 192.168.43.92：8888
    //Content-Type: application/x-www-urlencoded
    //Connection: Keep-Alive
    p = "GET / HTTP/1.1\r\nHost: 192.168.43.92:8888\r\nContent-Type: "
        "application/x-www-form-urlencoded\r\nConnection: Keep-Alive\r\n\r\n";
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == 0) {
        setSocketNonBlocking(sockfd);
        std::cout << "3:" << std::endl;
        ssize_t n = write(sockfd, p, strlen(p));
        std::cout << "strlen(p)= " << strlen(p) << std::endl;
        sleep(1);
        n = read(sockfd, buff, 4096);
        std::cout << "n= " << n << std::endl;
        printf("%s", buff);
        close(sockfd);
    } else {
        perror("err3");
    }
    return 0;
}