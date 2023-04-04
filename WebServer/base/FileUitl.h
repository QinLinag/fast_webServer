#ifndef __FILEUTIL_H__
#define __FILEUTIL_H__

#include<string>
#include "noncopyable.h"

class AppendFile : noncopyable {
public:
    explicit AppendFile(std::string filename);
    ~AppendFile();

    //append 向文件写入内容
    void append(const char* logline, const size_t len);

    void flush();

private:
    size_t write(const char* logline, size_t len);
    FILE* fp_;
    char buffer_[64 * 1024];
};


#endif