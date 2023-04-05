#ifndef __LOGSTREAM_H__
#define __LOGSTREAM_H__

#include <assert.h>
#include <string.h>
#include <string>
#include "noncopyable.h"

class AsyncLogging;
const int kSmallBuffer = 4096;
const int kLargeBuffer = 4096 * 1000;

template <int SIZE>
class FixedBuffer {
public:
    FixedBuffer() 
        : m_cur(m_data){
    }

    ~FixedBuffer() {}

    void append(const char* logline, size_t len) {
        if(avail() > static_cast<int>(len)) {
            memcpy(m_cur, logline, len);
            m_cur += len;
        }
    }
    
    const char* getData() const { return m_data;}
    int getLength() const { return static_cast<int>(m_cur - m_data);}

    char* getCur() { return m_cur;}
    int avail() const { return static_cast<int>(end() - m_cur);}
    void add(size_t len) { m_cur += len;}

    void reset() { m_cur = m_data;}
    void bzero() { memset(m_data, sizeof m_data, 0);}
private:
    const char* end() const { return m_data + sizeof m_data;}
    char m_data[SIZE];
    char* m_cur;
};

class LogStream : noncopyable{
public:
    typedef FixedBuffer<kSmallBuffer> Buffer;

    LogStream& operator<<(bool v) {
        m_buffer.append(v ? "1" : "0", 1);
        return *this;
    }

    LogStream& operator<<(short);
    LogStream& operator<<(unsigned short);
    LogStream& operator<<(int);
    LogStream& operator<<(unsigned int);
    LogStream& operator<<(long);
    LogStream& operator<<(unsigned long);
    LogStream& operator<<(long long);
    LogStream& operator<<(unsigned long long);

    LogStream& operator<<(const void*);

    LogStream& operator<<(float v) {
        *this << static_cast<double>(v);
        return *this;
    }

    LogStream& operator<<(double);
    LogStream& operator<<(long double);

    LogStream& operator<<(char v) {
        m_buffer.append(&v, 1);
        return *this;
    }

    LogStream& operator<<(const char* str) {
        if(str) {
            m_buffer.append(str, strlen(str));
        } else {
            m_buffer.append("<null>", 6);
            return *this;
        }
    }

    LogStream& operator<<(const unsigned char* str) {
        return operator<<(reinterpret_cast<const char*>(str));
    }

    LogStream& operator<<(const std::string& v) {
        if(!v.empty()) {
            m_buffer.append(v.c_str(), v.size());
            return *this;
        } else {
            m_buffer.append("<null>", 6);
            return *this;
        }
    }

    void append(const char* data, int len) { m_buffer.append(data, len);}
    const Buffer& getBuffer() const { return m_buffer;}
    void resetBuffer() { m_buffer.reset();}
private:
    void staticCheck();
    
    template<typename T>
    void formatInteger(T);

    Buffer m_buffer;

    static const int kMaxNumericSize = 32;
};


#endif