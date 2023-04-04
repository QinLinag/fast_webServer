#include "LogFile.h"
#include <assert.h>
#include <stdio.h>
#include <time.h>

LogFile::LogFile(const std::string& basename, int flushEveryN) 
    : m_basename(basename)
    , m_flushEveryN(flushEveryN)
    , m_count(0) {
    m_mutex.reset(new MutexLock);
    m_file.reset(new AppendFile(m_basename));
}

LogFile::~LogFile() {

}


void LogFile::append(const char* logline, int len) {
    MutexLockGuard(*m_mutex);
    append_unlock(logline, len);
}

void LogFile::flush() {
    MutexLockGuard(*m_mutex);
    m_file->flush();
}
bool LogFile::rollFile() {

}


void LogFile::append_unlock(const char* logline, int len) {
    m_file->append(logline, len);
    ++m_count;
    if(m_count >= m_flushEveryN) {
        m_count = 0;
        flush();
    }   
}