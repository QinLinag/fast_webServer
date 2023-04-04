//@Author Qin Liang
//@Email 2874974475@qq.com
#ifndef __ASYNCLOGGING_H__
#define __ASYNCLOGGING_H__

#include <functional>
#include <string>
#include <vector>
#include "CountDownLatch.h"
#include "LogStream.h"
#include "Thread.h"
#include "noncopyable.h"

class AsyncLogging : noncopyable {
public:
    AsyncLogging(const std::string& basename, int flushInterval = 2);
    ~AsyncLogging() {
        if(m_running) {
            stop();
        }
    }

    void append(const char* logline, int len);

    void start() {
        m_running = true;
        m_thread.start();
        m_latch.wait();
    }

    void stop() {
        m_running = false;
        m_cond.notify();
        m_thread.join();
    }

private:
    void threadFunc();  //给Thread的func函数，  异步将m_buffers缓冲中的内容写道文件中去
    typedef FixedBuffer<kLargeBuffer> Buffer;
    typedef std::shared_ptr<Buffer> BufferPtr;
    typedef std::vector<BufferPtr> BufferVector;
    const int m_flushInterval;
    bool m_running;
    std::string m_basename;
    Thread m_thread;
    MutexLock m_mutex;
    Condition m_cond;
    BufferPtr m_currentBuffer;
    BufferPtr m_nextBuffer;
    BufferVector m_buffers;
    CountDownLatch m_latch;
};


#endif