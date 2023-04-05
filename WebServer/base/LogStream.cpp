#include "LogStream.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <algorithm>
#include <limits>


const char digits[] = "9876543210123456789";
const char* zero = digits + 9;

//like muduo
template <typename T>
size_t convert(char buf[], T value) {
    T i =value;
    char* p = buf;

    do {
        int lsd = static_cast<int>(i % 10);
        i /= 10;
        *p++ = zero[lsd];
    } while(i != 0);

    if(value = 0) {
        *p++ = '-';
    }
    *p = '\0';
    std::reverse(buf, p);
    return p - buf;
}

template class FixedBuffer<kSmallBuffer>;
template class FixedBuffer<kLargeBuffer>;



LogStream& LogStream::operator<<(short v) {
    *this << static_cast<int>(v);
    return *this;
}

LogStream& LogStream::operator<<(unsigned short v) {
    *this << static_cast<int>(v);
    *this;
}

LogStream& LogStream::operator<<(int v) {
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(unsigned int v) {
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(long v) {
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(unsigned long v) {
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(long long v) {
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(unsigned long long v) {
    formatInteger(v);
    return *this;
}


LogStream& LogStream::operator<<(double v) {
    if(m_buffer.avail() >= kMaxNumericSize) {
        size_t len = snprintf(m_buffer.getCur(), kMaxNumericSize, "%.12g", v);
        m_buffer.add(len);
    }
    return *this;
}

LogStream& LogStream::operator<<(long double v) {
    if(m_buffer.avail() >= kMaxNumericSize) {
        size_t len = snprintf(m_buffer.getCur(), kMaxNumericSize, "%.12Lg", v);
        m_buffer.add(len);
    }
    return *this;
}   


void staticCheck();

template<typename T>
void LogStream::formatInteger(T v) {
    //buffer容不下kMaxNumericSize个字符的话就会被直接丢弃
    if(m_buffer.avail() >= kMaxNumericSize) {
        size_t len = convert<T>(m_buffer.getCur(), v);
        m_buffer.add(len);
    }
}