#ifndef __HTTPDATA_H__
#define __HTTPDATA_H__

#include <sys/epoll.h>
#include <unistd.h>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include "Timer.h"

class EventLoop;
class TimerNode;
class Channel;

enum ProcessState{
    STATE_PARSE_URI = 1,
    STATE_PARSE_HEADERS,
    STATE_RECV_BODY,
    STATE_ANALYSIS,
    STATE_FINISH
};

enum URIState {
    PARSE_URI_AGAIN = 1,
    PARSE_URI_ERROR,
    PARSE_URI_SUCCESS,
};

enum HeaderState {
    PARSE_HEADER_SUCCESS = 1,
    PARSE_HEADER_AGAIN,
    PARSE_HEADER_ERROR
};

enum AnalysisState {
    ANALYSIS_SUCCESS = 1, 
    ANALYSIS_ERROR
};

enum ParseState {
    H_START = 0,
    H_KEY,
    H_COLON,
    H_SPACES_AFTER_COLON,
    H_VALUE,
    H_CR,
    H_LF,
    H_END_CR,
    H_END_LF
};


enum ConnectionState {
    H_CONNECTED = 0,
    H_DISCONNECTING,
    H_DISCONNECTED
};

enum HttpMethod {
    METHOD_POST = 1,
    METHOD_GET, 
    METHOD_HEAD
};

enum HttpVersion {
    HTTP_10 = 1,
    HTTP_11
};

class MimeType {
private:
    static void init();
    static std::unordered_map<std::string, std::string> mime;
    MimeType();
    MimeType(const MimeType& m);

public:
    static std::string getMime(const std::string& suffix);

private:
    static pthread_once_t once_control;
};

class HttpData : public std::enable_shared_from_this<HttpData> {
public:
    HttpData(EventLoop* loop, int connfd);
    ~HttpData() {
        close(m_fd);
    }
    void reset();
    void seperateTimer();
    void linkTimer(std::shared_ptr<TimerNode> mtimer) {
        m_timer = mtimer;
    }
    std::shared_ptr<Channel> getChannel() {
        return m_channel;
    }
    EventLoop* getLoop() {
        return m_loop;
    }
    void handleClose();
    void newEvent();
 

private:
    EventLoop* m_loop;
    std::shared_ptr<Channel> m_channel;
    int m_fd;
    std::string m_inBuffer;
    std::string m_outBuffer;
    bool m_error;
    ConnectionState m_connectionState;

    HttpMethod m_method;
    HttpVersion m_HTTPVersion;
    std::string m_fileName;
    std::string m_path;
    int m_nowReadPos;
    ProcessState m_state;
    ParseState m_hState;
    bool m_keepAlive;
    std::map<std::string, std::string> m_headers;
    std::weak_ptr<TimerNode> m_timer;

    void handleRead();
    void handleWrite();
    void handleConn();
    void handleError(int fd, int err_num, std::string short_msg);
    URIState parseURI();
    HeaderState parseHeaders();
    AnalysisState analysisRequest();
};



















#endif