#include "Logging.h"
#include "CurrentThread.h"
#include "Thread.h"
#include "AsyncLogging.h"
#include <assert.h>
#include <iostream>
#include <time.h>
#include <sys/time.h>

static pthread_once_t once_control = PTHREAD_ONCE_INIT;
static AsyncLogging* AsycnLogger;

std::string Logger::m_logFileName = "./WebServer.log";

void once_init() {
    AsycnLogger = new AsyncLogging(Logger::getLogFileName());
    AsycnLogger->start();
}

void output(const char* msg, int len) {
    pthread_once(&once_control, once_init);  //只会执行一次，
    AsycnLogger->append(msg, len);
}

Logger::Logger(const char* fileName, int line) 
    : m_impl(fileName, line){
}

Logger::~Logger() {
    m_impl.m_stream << "--" << m_impl.m_basename
        << ":" << m_impl.m_line << "\n";
    const LogStream::Buffer& buf(stream().getBuffer());
    output(buf.getData(), buf.getLength());
}

Logger::Impl::Impl(const char* fileName, int line)
    : m_stream()
    , m_line(line)
    , m_basename(fileName) {
    formatTime();
}

void Logger::Impl::formatTime() {
    struct timeval tv;
    time_t time;
    char str_t[26] = {0};
    gettimeofday(&tv, NULL);
    time = tv.tv_sec;
    struct tm* p_time = localtime(&time);
    strftime(str_t, 26, "%Y-%m-%d %H:%M:%S\N", p_time);
    m_stream << str_t;
}