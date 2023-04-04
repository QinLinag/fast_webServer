#include "AsyncLogging.h"
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <functional>
#include "LogFile.h"

AsyncLogging::AsyncLogging(const std::string& basename, int flushInterval) 
    : m_flushInterval(flushInterval)
    , m_running(false)
    , m_basename(basename)
    , m_thread(std::bind(&AsyncLogging::threadFunc, this), "Logging")
    , m_mutex()
    , m_cond(m_mutex)
    , m_buffers()
    , m_latch(1){
    m_currentBuffer.reset(new Buffer);
    m_nextBuffer.reset(new Buffer);
    assert(basename.size() > 1);
    m_currentBuffer->bzero();
    m_nextBuffer->bzero();
    m_buffers.reserve(16);
}

void AsyncLogging::append(const char* logline, int len) {
    MutexLockGuard lock(m_mutex);
    if(m_currentBuffer->avail() > len) {
        m_currentBuffer->append(logline, len);
    } else {
        m_buffers.push_back(m_currentBuffer);
        m_currentBuffer.reset();
        if(m_nextBuffer) {
            m_currentBuffer = std::move(m_nextBuffer);
        } else {
            m_currentBuffer.reset(new Buffer);
        }
        m_currentBuffer->append(logline, len);
        m_cond.notify();
    }
}

void AsyncLogging::threadFunc() {
    assert(m_running == true);
    m_latch.countDown();
    
    LogFile output(m_basename);
    
    BufferPtr newBuffer1(new Buffer);
    BufferPtr newBuffer2(new Buffer);
    newBuffer1->bzero();
    newBuffer2->bzero();

    BufferVector buffersToWrite;
    buffersToWrite.reserve(16);
    while(m_running) {
        assert(newBuffer1 && newBuffer1->getLength() == 0);
        assert(newBuffer2 && newBuffer2->getLength() == 0);
        assert(buffersToWrite.empty());

        {
            MutexLockGuard lock(m_mutex);
            if(m_buffers.empty()) {
                m_cond.waitForSeconds(m_flushInterval);
            }
            m_buffers.push_back(m_currentBuffer);
            m_currentBuffer.reset();

            m_currentBuffer = std::move(newBuffer1);
            buffersToWrite.swap(m_buffers);
            if(!m_nextBuffer) {
                m_nextBuffer = std::move(newBuffer2);
            }
        }

        assert(!buffersToWrite.empty());

        if(buffersToWrite.size() > 25) {
            buffersToWrite.erase(buffersToWrite.begin() + 2, buffersToWrite.end());
        }
        for(size_t i = 0; i < buffersToWrite.size(); ++i) {
            output.append(buffersToWrite[i]->getData(), buffersToWrite[i]->getLength());
        }
        if(buffersToWrite.size() > 2) {
            buffersToWrite.resize(2);
        }

        if(!newBuffer1) {
            assert(!buffersToWrite.empty());
            newBuffer1 = buffersToWrite.back();
            buffersToWrite.pop_back();
            newBuffer1->reset();
        }
        if(!newBuffer2) {
            assert(!buffersToWrite.empty());
            newBuffer2 = buffersToWrite.back();
            buffersToWrite.pop_back();
            newBuffer2->reset();
        }

        buffersToWrite.clear();
        output.flush();
    }
    output.flush();
}

