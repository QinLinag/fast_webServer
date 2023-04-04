#ifndef __COUNTDOWNLATCH_H__
#define __COUNTDOWNLATCH_H__

#include "Condition.h"
#include "MutexLock.h"
#include "noncopyable.h"

class CountDownLatch : noncopyable {
public:
    explicit CountDownLatch(int count);
    void wait();
    void countDown();
private:
    mutable MutexLock m_mutex;
    Condition m_condition;
    int m_count;
};


#endif