#include "Thread.h"
#include "CurrentThread.h"
#include <string>
#include <memory>
#include <stdint.h>
#include <stdio.h>
#include <sys/prctl.h>
#include <sys/types.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <linux/unistd.h>
#include <iostream>

namespace CurrentThread{
    __thread int t_cachedTid = 0;
    __thread char t_tidString[32];
    __thread int t_tidStringLength = 6;
    __thread const char* t_threadName = "default";
}

pid_t getTid() { return static_cast<pid_t>(::syscall(SYS_gettid));}

void CurrentThread::cacheTid() {
    if(t_cachedTid == 0) {
        t_cachedTid = gettid();
        t_tidStringLength = 
            snprintf(t_tidString, sizeof t_tidString, "%5d", t_cachedTid);
    }
}

//为了在执行线程时可以使用到name，tid等数据， 也就是pthread_create函数的data参数，
struct ThreadData {
    typedef Thread::ThreadFunc ThreadFunc;
    ThreadFunc m_func;
    std::string m_name;
    pid_t* m_tid;
    CountDownLatch* m_latch;

    ThreadData(const ThreadFunc& func, 
        const std::string& name, pid_t* tid,
        CountDownLatch* latch) 
        : m_name(name)
        , m_tid(tid)
        , m_latch(latch){
    std::cout << "m_func==func at ThreadData" << std::endl;
    m_func = func;
    }

    void runInThread() {
        *m_tid = CurrentThread::tid();
        m_tid = nullptr;
        m_latch->countDown();
        m_latch = nullptr;
        CurrentThread::t_threadName = m_name.empty() ? "Thread" : m_name.c_str();
        prctl(PR_SET_NAME, CurrentThread::t_threadName);
        m_func();
        CurrentThread::t_threadName = "finished";
    }

};


void* startThread(void* obj) {
    ThreadData* data = (ThreadData*)obj;
    data->runInThread();
    delete data;
    return nullptr;
}

Thread::Thread(const ThreadFunc& func, const std::string& name) 
    : m_started(false)
    , m_joined(false)
    , m_pthread(0)
    , m_tid(0)
    , m_func(func)
    , m_name(name)
    , m_latch(1){
    setDefaultName();
}

Thread::~Thread() {
    if(m_started && !m_joined) {
        pthread_detach(m_pthread);
    }
}

void Thread::start() {
    assert(!m_started);
    m_started = true;
    ThreadData* data = new ThreadData(m_func, m_name, &m_tid, &m_latch);
    int rt = pthread_create(&m_pthread, nullptr, &startThread, data);
    if(rt == -1) {
        m_started = false;
        delete data;
    } else {
        m_latch.wait();
        assert(m_tid > 0);
    }
}

int Thread::join() {
    assert(m_started);
    assert(!m_joined);
    m_joined = true;
    pthread_join(m_pthread, nullptr);
}

void Thread::setDefaultName() {
    if(m_name.empty()) {
        char buf[32];
        snprintf(buf, sizeof buf, "Thread");
        m_name = buf;
    }
}
