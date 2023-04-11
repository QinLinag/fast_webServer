#ifndef __EVENTLOOP_H__
#define __EVENTLOOP_H__

#include <functional>
#include <memory>
#include <vector>
#include "Channel.h"
#include "Epoll.h"
#include "base/CurrentThread.h"
#include "base/Logging.h"
#include "base/Thread.h"
#include "Util.h"

#include <iostream>

class EventLoop {
public:
    typedef std::function<void()> Functor;
    EventLoop();
    ~EventLoop();
    void loop();
    void quit();
    void runInLoop(Functor&& cb);
    void queueInLoop(Functor&& cb);
    bool isInLoopThread() const { return m_threadId == CurrentThread::tid();}
    void assertInLoopThread() { assert(isInLoopThread());}//EventLoop是某一个线程所拥有的，只属于创建EventLoop对象的线程所拥有，当某个线程执行eventloop时，需要断言是否是该线程拥有
    void shutDown(std::shared_ptr<Channel> channel) { shutDownWR(channel->getFd());}
    void removeFromPoller(std::shared_ptr<Channel> channel) {
        m_poller->epoll_del(channel);
    }
    void updatePoller(std::shared_ptr<Channel> channel, int timeout = 0) {
        m_poller->epoll_mod(channel, timeout);
    }
    void addToPoller(std::shared_ptr<Channel> channel, int timeout) {
        m_poller->epoll_add(channel, timeout); 
    }

private:
    bool m_looping;
    std::shared_ptr<Epoll> m_poller;
    int m_wakeupFd;
    bool m_quit;
    bool m_eventHandling;
    mutable MutexLock m_mutex;
    std::vector<Functor> m_pendingFunctors;
    bool m_callingPendingFunctors;
    const pid_t m_threadId;
    std::shared_ptr<Channel> m_pwakeupChannel;

    void wakeup();
    void handleRead();
    void doPendingFunctors();
    void handleConn();
};


#endif