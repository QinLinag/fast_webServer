#ifndef __CONDITION_H__
#define __CONDITION_H__

#include <errno.h>
#include <pthread.h>
#include <time.h>
#include <cstdint>
#include "MutexLock.h"
#include "noncopyable.h"

class Condition : noncopyable {
public:
    Condition(MutexLock& mutex) 
     : m_mutex(mutex){
    pthread_cond_init(&m_cond, nullptr);
    }

    ~Condition() { pthread_cond_destroy(&m_cond);}

    void wait() { pthread_cond_wait(&m_cond, m_mutex.get());}
    void notify() { pthread_cond_signal(&m_cond);}
    void notifyAll() { pthread_cond_broadcast(&m_cond);}

    bool waitForSeconds(int seconds) {
        struct timespec abstime;
        clock_gettime(CLOCK_REALTIME, &abstime);
        abstime.tv_sec += static_cast<time_t>(seconds);
        return ETIMEDOUT 
                == pthread_cond_timedwait(&m_cond, m_mutex.get(), &abstime);
    }

private:
    pthread_cond_t m_cond;
    MutexLock& m_mutex;
};



#endif