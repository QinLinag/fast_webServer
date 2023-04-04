#ifndef __THREAD_H__
#define __THREAD_H__

#include <pthread.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <functional>
#include <memory>
#include <string>
#include "CountDownLatch.h"
#include "noncopyable.h"



class Thread : noncopyable{
public:
    typedef std::function<void()> ThreadFunc;
    explicit Thread(const ThreadFunc& func, const std::string& name = std::string());
    ~Thread();
    void start();
    int join();
    bool getStarted() const { return m_started;}
    pid_t getTid() const { return m_tid;}
    const std::string& getName() const { return m_name;} 
private:
    void setDefaultName();
    pthread_t m_pthread;
    bool m_started;
    bool m_joined;
    pid_t m_tid;
    ThreadFunc m_func;
    std::string m_name;
    CountDownLatch m_latch;
};


#endif