#ifndef __LOGGING_H__
#define __LOGGING_H__

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include "LogStream.h"


class AsyncLogging;

class Logger{
public:
    Logger(const char* fileName, int line);
    ~Logger();

    LogStream& stream() { return m_impl.m_stream;}

    static void setLogFileName(std::string fileName) { m_logFileName = fileName;}
    static std::string getLogFileName() { return m_logFileName;}
private:
    class Impl{
    public:
        Impl(const char* fileName, int line);
        void formatTime();

        LogStream m_stream;
        int m_line;
        std::string m_basename;
    };

private:
    Impl m_impl;
    static std::string m_logFileName;
};

#endif