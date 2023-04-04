#ifndef __LOGFILE_H__
#define __LOGFILE_H__

#include <memory>
#include <string>
#include "FileUitl.h"
#include "MutexLock.h"
#include "noncopyable.h"

class LogFile : noncopyable {
public:
    LogFile(const std::string& basename, int flushEveryN = 1024);
    ~LogFile();

    void appen(const char* logline, int len);
    void flush();
    bool rollFile();
private:
    void append_unlock(const char* logline, int len);
    const std::string m_basename;
    const int m_flushEveryN;
    int m_count;
    std::shared_ptr<MutexLock> m_mutex;
    std::shared_ptr<AppendFile> m_file;
};


#endif