#include "EventLoopThread.h"
#include <functional>




EventLoopThread::EventLoopThread() 
    : m_loop(nullptr)
    , m_exiting(false)
    , m_thread(std::bind(&EventLoopThread::threadFunc, this), "EventLoopThread")
    , m_mutex()
    , m_cond(m_mutex){
}

EventLoopThread::~EventLoopThread() {
    m_exiting = true;
    if(m_loop != nullptr) {
        m_loop->quit();
        m_thread.join();
    }
}

EventLoop* EventLoopThread::startLoop() {
    assert(!m_thread.getStarted());
    m_thread.start();
    {
        MutexLockGuard lock(m_mutex);
        //一直等到threadFun在thread中被执行，loop被创建才退出循环
        while(m_loop == nullptr) {
            m_cond.wait();
        }
    }
    return m_loop;
}

void EventLoopThread::threadFunc() {
    EventLoop loop;
    {
        MutexLockGuard lock(m_mutex);
        m_loop = &loop;
        m_cond.notify();
    }
    m_loop->loop();
    // delete m_loop;
    m_loop = nullptr;
}







