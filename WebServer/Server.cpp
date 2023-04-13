#include "Server.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <functional>
#include "Util.h"
#include "base/Logging.h"

Server::Server(EventLoop* loop, int threadNum, int port) 
    : m_loop(loop)
    , m_threadNum(threadNum)
    , m_eventLoopThreadPool(new EventLoopThreadPool(m_loop, threadNum))
    , m_started(false)
    , m_acceptChannel(new Channel(m_loop))
    , m_port(port)
    , m_listenFd(socket_bind_listen(m_port)){
    m_acceptChannel->setFd(m_listenFd);
    handle_for_sigpipe();
    std::cout << "at 19" << std::endl;
    if(setSocketNonBlocking(m_listenFd) < 0) {
        LOG << "set socket non block failed";
        perror("set socket non block failed");
        std::cout << "at 23" << std::endl;
        abort();
    }
}

void Server::start() {
    m_eventLoopThreadPool->start();
    m_acceptChannel->setEvents(EPOLLIN | EPOLLET);
    m_acceptChannel->setReadHandler(std::bind(&Server::handNewConn, this));
    m_acceptChannel->setConnHandler(std::bind(&Server::handThisConn, this));
    m_loop->addToPoller(m_acceptChannel, 0); //虽然Server start了，但是m_loop还没有start，也就是说m_loop虽然add了accepChannel,但是还不能接受客户端的连接，
                                            //m_loop在main函数中手动start，此时才真正的开始接受客户端的连接
    m_started = true;
}

void Server::handNewConn() {
    struct sockaddr_in client_addr;
    memset(&client_addr, 0, sizeof(struct sockaddr_in));
    socklen_t client_addr_len = sizeof(client_addr);
    int accept_fd = 0;
    while((accept_fd = accept(m_listenFd, (struct sockaddr*)&client_addr,
                &client_addr_len)) > 0) {
        EventLoop* loop = m_eventLoopThreadPool->getNextLoop();
        LOG << "New connection from " << inet_ntoa(client_addr.sin_addr)
            << ":" << ntohs(client_addr.sin_port);
        if(accept_fd >= MAXFDS) {
            LOG << "more than MAXFDS";
            close(accept_fd);
            continue;
        }
        if(setSocketNonBlocking(accept_fd) < 0) {
            LOG << "Set non block failed";
            return;
        }
        setSocketNodelay(accept_fd);

        std::shared_ptr<HttpData> req_info(new HttpData(loop, accept_fd));
        req_info->getChannel()->setHolder(req_info);
        loop->queueInLoop(std::bind(&HttpData::newEvent, req_info));//在这里将新的连接挂上树
    }
    m_acceptChannel->setEvents(EPOLLIN | EPOLLET);
}
