#ifndef __SERVER_H__
#define __SERVER_H__

#include <memory>
#include "Channel.h"
#include "EventLoop.h"
#include "EventLoopThreadPool.h"

class Server{
public:
    Server(EventLoop* loop, int threadNum, int port);
    ~Server(){}
    EventLoop* getLoop() const { return m_loop;}
    void start();
    void handNewConn();
    void handThisConn() { m_loop->updatePoller(m_acceptChannel);}

private:
    EventLoop* m_loop;
    int m_threadNum;
    std::unique_ptr<EventLoopThreadPool> m_eventLoopThreadPool;
    bool m_started;
    std::shared_ptr<Channel> m_acceptChannel; //用于服务器处理客户端连接的channel
    int m_port;
    int m_listenFd;
    static const int MAXFDS = 100000;
};

#endif