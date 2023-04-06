#include "Channel.h"
#include <unistd.h>
#include <cstdlib>
#include <iostream>

#include <queue>
#include "Epoll.h"
#include "EventLoop.h"
#include "Util.h"



Channel::Channel(EventLoop* loop) 
    : m_loop(loop)
    , m_events(0)
    , m_lastEvents(0)
    , m_fd(0){
}

Channel::Channel(EventLoop* loop, int fd) 
    : m_loop(loop)
    , m_fd(fd)
    , m_events(0)
    , m_lastEvents(0){
}

Channel::~Channel() {
    //这里不能将fd close掉，因为fd不属于Channel
}

void Channel::handleRead() {
    if(m_readHandler) {
        m_readHandler();
    }
}

void Channel::handleWrite() {
    if(m_writeHandler) {
        m_writeHandler();
    }
}

void Channel::handleError(int fd, int err_num, 
                            std::string short_msg) {
    if(m_errorHandler) {
        m_errorHandler();
    }
}

void Channel::handleConn() {
    if(m_connHandler) {
        m_connHandler();
    }
}

