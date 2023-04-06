#ifndef __EPOLL_H__
#define __EPOLL_H__

#include <sys/epoll.h>
#include <memory>
#include <unordered_map>
#include <vector>
#include "Channel.h"
#include "HttpData.h"
#include "Timer.h"

class Epoll {
public:
    Epoll();
    ~Epoll();
    void epoll_add(SP_Channel request, int timeout);
    void epoll_mod(SP_Channel request, int timeout);
    void epoll_del(SP_Channel reqeust);
    std::vector<std::shared_ptr<Channel> > poll(); //执行epoll_wait
    std::vector<std::shared_ptr<Channel> > getEventsReqeust(int events_num);
    void add_timer(std::shared_ptr<Channel> request_data, int timeout);
    int getEpollFd() { return m_epollFd;}
    void handleExpired();

private:
    int m_epollFd;
    static const int MAXFDS = 100000;
    std::vector<epoll_event> m_events;
    std::shared_ptr<Channel> m_fd2chan[MAXFDS];
    std::shared_ptr<HttpData> m_fd2http[MAXFDS];
    TimerManager m_timerManager;
};

#endif