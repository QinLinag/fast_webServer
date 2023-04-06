#include "Epoll.h"
#include <assert.h>
#include <errno.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <deque>
#include <queue>
#include "Util.h"
#include "base/Logging.h"

#include <arpa/inet.h>
#include <iostream>

const int EVENTSNUM = 4096;
const int EPOLLWAIT_TIME = 10000;


Epoll::Epoll() 
    : m_epollFd(epoll_create1(EPOLL_CLOEXEC))
    , m_events(EVENTSNUM){
    assert(m_epollFd > 0);
}

Epoll::~Epoll() {
}

void Epoll::epoll_add(SP_Channel request, int timeout) {
    int fd = request->getFd();
    if(timeout > 0) {
        add_timer(request, timeout);
    }
    m_fd2http[fd] = request->getHolder();
    m_fd2chan[fd] = request;
    struct epoll_event event;
    event.data.fd = fd;
    event.events = request->getEvents();

    request->EqualAndUpdateLastEvents();

    int rt = epoll_ctl(m_epollFd, EPOLL_CTL_ADD, fd, &event);
    if(rt < 0) {
        LOG << "epoll_ctl for add failed";
        perror("epoll_ctl for add failed");
        m_fd2chan[fd].reset();
        m_fd2http[fd].reset();
    }
}

void Epoll::epoll_mod(SP_Channel request, int timeout) {
    int fd = request->getFd();
    if(timeout > 0) {
        add_timer(request, timeout);
    }
    if(!request->EqualAndUpdateLastEvents()) { //这次事件和上次事件如果不同
        struct epoll_event event;
        event.data.fd = fd;
        event.events = request->getEvents();
        int rt = epoll_ctl(m_epollFd, EPOLL_CTL_MOD, fd, &event);
        if(rt < 0) {
            LOG << "epoll_ctl for mod failed";
            perror("epoll_ctl for mod failed");
            m_fd2chan[fd].reset();
            m_fd2http[fd].reset();
        }
    }
}

void Epoll::epoll_del(SP_Channel reqeust) {
    int fd = reqeust->getFd();
    struct epoll_event event;
    event.data.fd = fd;
    event.events = reqeust->getLastEvents();

    int rt = epoll_ctl(m_epollFd, EPOLL_CTL_DEL, fd, &event);
    if(rt < 0) {
        LOG << "epoll_ctl for del failed";
        perror("epoll_ctl for del failed");
    }
    m_fd2chan[fd].reset();
    m_fd2http[fd].reset();
}

//返回活跃事件
std::vector<std::shared_ptr<Channel> > Epoll::poll() {
    while(true) {
        int event_count = 
            epoll_wait(m_epollFd, &(*m_events.begin()),
                m_events.size(), EPOLLWAIT_TIME);
        if(event_count < 0) {
            LOG << "epoll_wait failed";
            perror("epoll_wait failed");
        }
        std::vector<SP_Channel> ret_data = getEventsReqeust(event_count);
        if(ret_data.size() > 0) {  //没有事件，就继续epoll_wait
            return ret_data;
        }
    }
}

std::vector<std::shared_ptr<Channel> > Epoll::getEventsReqeust(int events_num) {
    std::vector<SP_Channel> req_data;
    for(int i = 0; i < events_num; ++i) {
        //获取有事件产生的描述符
        int fd = m_events[i].data.fd;
        SP_Channel cur_req = m_fd2chan[fd];
        if(cur_req) {
            cur_req->setRevents(m_events[i].events);
            cur_req->setEvents(0);
            req_data.push_back(cur_req);                
        } else {
            LOG << "getEventRequest SP cur_req is invalid";
        }
    }
    return req_data;
}

void Epoll::add_timer(std::shared_ptr<Channel> request_data, int timeout) {
    std::shared_ptr<HttpData> t = request_data->getHolder();
    if(t) {
        m_timerManager.addTimer(t, timeout);
    } else {
        LOG << "epoll add_timer failed";
    }
}

void Epoll::handleExpired() {
    m_timerManager.handleExpiredEvent();
}
