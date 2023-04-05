#ifndef __MUTEXLOCK_H__
#define __MUTEXLOCK_H__

#include <pthread.h>
#include <cstdio>
#include "noncopyable.h"

class MutexLock : noncopyable{
friend class Condition;
public:
    MutexLock() {
        pthread_mutex_init(&m_mutex, nullptr);
    }

    ~MutexLock() {
        pthread_mutex_lock(&m_mutex);
        pthread_mutex_destroy(&m_mutex);
    }

    pthread_mutex_t* get()  { return &m_mutex;}

    void lock() {
        pthread_mutex_lock(&m_mutex);
    }

    void unlock() {
        pthread_mutex_unlock(&m_mutex);
    }

private:
    pthread_mutex_t m_mutex;
};

class MutexLockGuard {
public:
    explicit MutexLockGuard(MutexLock& mutex) 
        :m_mutex(mutex){
        m_mutex.lock();
    }

    ~MutexLockGuard() {
        m_mutex.unlock();
    }

private:
    MutexLock &m_mutex;
};


#endif