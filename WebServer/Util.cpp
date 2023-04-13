#include "Util.h"
#include <iostream>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <signal.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

const int MAX_BUFF = 4096;



ssize_t readn(int fd, void* buff, size_t n) {
    size_t nleft = n;
    ssize_t nread = 0;
    ssize_t readSum = 0;
    char* ptr = (char*)buff;
    while(nleft > 0) {
        nread = read(fd, ptr, nleft);
        if(nread < 0) {
            if(errno == EINTR) {
                nread = 0;
            } else if(errno == EAGAIN) {
                return readSum;
            } else {
                return -1;
            }
        } else if (nread == 0) {
            break; //读完了，
        }
        readSum += nread;
        nleft -= nread;
        ptr += nread;
    }
    return readSum;
}


ssize_t readn(int fd, std::string& inBuffer, bool& zero) {
    ssize_t nread = 0;
    ssize_t readSum = 0;
    while (true) {
        char buff[MAX_BUFF];    //每个循环开始就开辟一片空间，
        nread = read(fd, buff, MAX_BUFF);
        if(nread < 0) {
            if(errno == EINTR) {
                continue;
            } else if(errno == EAGAIN) {
                return readSum;
            } else {
                perror("read error");
                return -1;
            }
        } else if(nread == 0) {
            zero = true;
            break;
        }
        readSum += nread;
        inBuffer += std::string(buff, buff + nread); //buff每个循环都是新的
    }
    return readSum;
}

ssize_t readn(int fd, std::string& inBuffer) {
    ssize_t nread = 0;
    ssize_t readSum = 0;
    while(true) {
        char buff[MAX_BUFF];
        nread = read(fd, buff, MAX_BUFF);
        if(nread < 0) {
            if(errno == EINTR) {
                continue;
            } else if(errno = EAGAIN) {
                return readSum;
            } else {
                perror("read error");
                return -1;
            }
        } else if(nread == 0) {
            break;
        }
        readSum += nread;
        inBuffer += std::string(buff, buff + nread);
    }
    return readSum;
}

ssize_t writen(int fd, void* buff, size_t n) {
    size_t nleft = n;
    ssize_t nwritten = 0;
    ssize_t writeSum = 0;
    char* ptr = (char*)buff;
    while(nleft > 0) {
        nwritten = write(fd, ptr, nleft);
        if(nwritten < 0) {
            if(errno == EINTR) {
                nwritten = 0;
                continue;
            } else if(errno == EAGAIN) {
                return writeSum;
            } else {
                return -1;
            }
        }
        writeSum += nwritten;
        nleft -= nwritten;
        ptr += nwritten;
    }
    return writeSum;
}

ssize_t writen(int fd, std::string& sbuff) {
    ssize_t nleft = sbuff.size();
    ssize_t nwritten = 0;
    ssize_t writeSum = 0;
    const char* ptr = sbuff.c_str();
    while(nleft > 0) {
        nwritten = write(fd, ptr, nleft);
        if(nwritten < 0) {
            if(errno == EINTR) {
                nwritten = 0;
                continue;
            } else if(errno == EAGAIN) {
                break;
            } else {
                return -1;
            }
        } else if(nwritten == 0) {
            break;
        }
        nleft -= nwritten;
        writeSum += nwritten;
        ptr += nwritten;
    }
    if(writeSum == static_cast<int>(sbuff.size())) {
        sbuff.clear();
    } else {
        sbuff = sbuff.substr(writeSum);
    }
    return writeSum;
}

void handle_for_sigpipe() {
    struct sigaction sa;
    memset(&sa, '\0', sizeof(sa));
    sa.sa_handler = SIG_IGN;
    sa.sa_flags = 0;
    if(sigaction(SIGPIPE, &sa, NULL)) {
        return ;
    }
}

int setSocketNonBlocking(int fd) {
    std::cout << "lalala"  << fd<< std::endl;
    int flag = fcntl(fd, F_GETFL, 0);
    std::cout << "lalala" << std::endl;
    if(flag == -1) {
        return -1;
    }
    flag |= O_NONBLOCK;
    if(fcntl(fd, F_SETFL, flag) == -1) {
        return -1;
    }
    std::cout << "lalala" << std::endl;
    return 0;
}

void setSocketNodelay(int fd) {
    int enable = 1;
    setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, 
        (void*)enable, sizeof(enable));
}

void setSocketNoLinger(int fd) {
    struct linger linger_;
    linger_.l_onoff = 1;
    linger_.l_linger = 30;
    setsockopt(fd, SOL_SOCKET, SO_LINGER, 
            (const char*)&linger_, sizeof(linger_));
}

void shutDownWR(int fd) {
    shutdown(fd, SHUT_WR);
}

int socket_bind_listen(int port) {
        // 检查port值，取正确区间范围
    if (port < 0 || port > 65535) return -1;

    // 创建socket(IPv4 + TCP)，返回监听描述符
    int listen_fd = 0;
    if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) return -1;

    // 消除bind时"Address already in use"错误
    int optval = 1;
    if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &optval,
                    sizeof(optval)) == -1) {
        close(listen_fd);
        return -1;
    }

    // 设置服务器IP和Port，和监听描述副绑定
    struct sockaddr_in server_addr;
    bzero((char *)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons((unsigned short)port);
    if (bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) ==-1) {
        std::cout << "bind == -1" << std::endl;
        close(listen_fd);
        return -1;
    }

    // 开始监听，最大等待队列长为LISTENQ
    if (listen(listen_fd, 2048) == -1) {
        close(listen_fd);
        return -1;
    }

    // 无效监听描述符
    if (listen_fd == -1) {
        close(listen_fd);
        return -1;
    }
    return listen_fd;
}