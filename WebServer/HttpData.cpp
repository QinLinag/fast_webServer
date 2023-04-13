#include "HttpData.h"
#include "Channel.h"
#include "EventLoop.h"
#include "time.h"
#include "Util.h"
#include <iostream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "base/Logging.h"

pthread_once_t MimeType::once_control = PTHREAD_ONCE_INIT;
std::unordered_map<std::string, std::string> MimeType::mime;

const __uint32_t DEFAULT_EVENT = EPOLLIN | EPOLLET |EPOLLONESHOT;
const int DEFAULT_EXPIRED_TIME = 2000;
const int DEFAULT_KEEP_ALIVE_TIME = 5 * 60 * 1000;

char favicon[555] {
     '\x89', 'P',    'N',    'G',    '\xD',  '\xA',  '\x1A', '\xA',  '\x0',
    '\x0',  '\x0',  '\xD',  'I',    'H',    'D',    'R',    '\x0',  '\x0',
    '\x0',  '\x10', '\x0',  '\x0',  '\x0',  '\x10', '\x8',  '\x6',  '\x0',
    '\x0',  '\x0',  '\x1F', '\xF3', '\xFF', 'a',    '\x0',  '\x0',  '\x0',
    '\x19', 't',    'E',    'X',    't',    'S',    'o',    'f',    't',
    'w',    'a',    'r',    'e',    '\x0',  'A',    'd',    'o',    'b',
    'e',    '\x20', 'I',    'm',    'a',    'g',    'e',    'R',    'e',
    'a',    'd',    'y',    'q',    '\xC9', 'e',    '\x3C', '\x0',  '\x0',
    '\x1',  '\xCD', 'I',    'D',    'A',    'T',    'x',    '\xDA', '\x94',
    '\x93', '9',    'H',    '\x3',  'A',    '\x14', '\x86', '\xFF', '\x5D',
    'b',    '\xA7', '\x4',  'R',    '\xC4', 'm',    '\x22', '\x1E', '\xA0',
    'F',    '\x24', '\x8',  '\x16', '\x16', 'v',    '\xA',  '6',    '\xBA',
    'J',    '\x9A', '\x80', '\x8',  'A',    '\xB4', 'q',    '\x85', 'X',
    '\x89', 'G',    '\xB0', 'I',    '\xA9', 'Q',    '\x24', '\xCD', '\xA6',
    '\x8',  '\xA4', 'H',    'c',    '\x91', 'B',    '\xB',  '\xAF', 'V',
    '\xC1', 'F',    '\xB4', '\x15', '\xCF', '\x22', 'X',    '\x98', '\xB',
    'T',    'H',    '\x8A', 'd',    '\x93', '\x8D', '\xFB', 'F',    'g',
    '\xC9', '\x1A', '\x14', '\x7D', '\xF0', 'f',    'v',    'f',    '\xDF',
    '\x7C', '\xEF', '\xE7', 'g',    'F',    '\xA8', '\xD5', 'j',    'H',
    '\x24', '\x12', '\x2A', '\x0',  '\x5',  '\xBF', 'G',    '\xD4', '\xEF',
    '\xF7', '\x2F', '6',    '\xEC', '\x12', '\x20', '\x1E', '\x8F', '\xD7',
    '\xAA', '\xD5', '\xEA', '\xAF', 'I',    '5',    'F',    '\xAA', 'T',
    '\x5F', '\x9F', '\x22', 'A',    '\x2A', '\x95', '\xA',  '\x83', '\xE5',
    'r',    '9',    'd',    '\xB3', 'Y',    '\x96', '\x99', 'L',    '\x6',
    '\xE9', 't',    '\x9A', '\x25', '\x85', '\x2C', '\xCB', 'T',    '\xA7',
    '\xC4', 'b',    '1',    '\xB5', '\x5E', '\x0',  '\x3',  'h',    '\x9A',
    '\xC6', '\x16', '\x82', '\x20', 'X',    'R',    '\x14', 'E',    '6',
    'S',    '\x94', '\xCB', 'e',    'x',    '\xBD', '\x5E', '\xAA', 'U',
    'T',    '\x23', 'L',    '\xC0', '\xE0', '\xE2', '\xC1', '\x8F', '\x0',
    '\x9E', '\xBC', '\x9',  'A',    '\x7C', '\x3E', '\x1F', '\x83', 'D',
    '\x22', '\x11', '\xD5', 'T',    '\x40', '\x3F', '8',    '\x80', 'w',
    '\xE5', '3',    '\x7',  '\xB8', '\x5C', '\x2E', 'H',    '\x92', '\x4',
    '\x87', '\xC3', '\x81', '\x40', '\x20', '\x40', 'g',    '\x98', '\xE9',
    '6',    '\x1A', '\xA6', 'g',    '\x15', '\x4',  '\xE3', '\xD7', '\xC8',
    '\xBD', '\x15', '\xE1', 'i',    '\xB7', 'C',    '\xAB', '\xEA', 'x',
    '\x2F', 'j',    'X',    '\x92', '\xBB', '\x18', '\x20', '\x9F', '\xCF',
    '3',    '\xC3', '\xB8', '\xE9', 'N',    '\xA7', '\xD3', 'l',    'J',
    '\x0',  'i',    '6',    '\x7C', '\x8E', '\xE1', '\xFE', 'V',    '\x84',
    '\xE7', '\x3C', '\x9F', 'r',    '\x2B', '\x3A', 'B',    '\x7B', '7',
    'f',    'w',    '\xAE', '\x8E', '\xE',  '\xF3', '\xBD', 'R',    '\xA9',
    'd',    '\x2',  'B',    '\xAF', '\x85', '2',    'f',    'F',    '\xBA',
    '\xC',  '\xD9', '\x9F', '\x1D', '\x9A', 'l',    '\x22', '\xE6', '\xC7',
    '\x3A', '\x2C', '\x80', '\xEF', '\xC1', '\x15', '\x90', '\x7',  '\x93',
    '\xA2', '\x28', '\xA0', 'S',    'j',    '\xB1', '\xB8', '\xDF', '\x29',
    '5',    'C',    '\xE',  '\x3F', 'X',    '\xFC', '\x98', '\xDA', 'y',
    'j',    'P',    '\x40', '\x0',  '\x87', '\xAE', '\x1B', '\x17', 'B',
    '\xB4', '\x3A', '\x3F', '\xBE', 'y',    '\xC7', '\xA',  '\x26', '\xB6',
    '\xEE', '\xD9', '\x9A', '\x60', '\x14', '\x93', '\xDB', '\x8F', '\xD',
    '\xA',  '\x2E', '\xE9', '\x23', '\x95', '\x29', 'X',    '\x0',  '\x27',
    '\xEB', 'n',    'V',    'p',    '\xBC', '\xD6', '\xCB', '\xD6', 'G',
    '\xAB', '\x3D', 'l',    '\x7D', '\xB8', '\xD2', '\xDD', '\xA0', '\x60',
    '\x83', '\xBA', '\xEF', '\x5F', '\xA4', '\xEA', '\xCC', '\x2',  'N',
    '\xAE', '\x5E', 'p',    '\x1A', '\xEC', '\xB3', '\x40', '9',    '\xAC',
    '\xFE', '\xF2', '\x91', '\x89', 'g',    '\x91', '\x85', '\x21', '\xA8',
    '\x87', '\xB7', 'X',    '\x7E', '\x7E', '\x85', '\xBB', '\xCD', 'N',
    'N',    'b',    't',    '\x40', '\xFA', '\x93', '\x89', '\xEC', '\x1E',
    '\xEC', '\x86', '\x2',  'H',    '\x26', '\x93', '\xD0', 'u',    '\x1D',
    '\x7F', '\x9',  '2',    '\x95', '\xBF', '\x1F', '\xDB', '\xD7', 'c',
    '\x8A', '\x1A', '\xF7', '\x5C', '\xC1', '\xFF', '\x22', 'J',    '\xC3',
    '\x87', '\x0',  '\x3',  '\x0',  'K',    '\xBB', '\xF8', '\xD6', '\x2A',
    'v',    '\x98', 'I',    '\x0',  '\x0',  '\x0',  '\x0',  'I',    'E',
    'N',    'D',    '\xAE', 'B',    '\x60', '\x82',
};



void MimeType::init() {
    mime[".html"] = "text/html";
    mime[".avi"] = "video/x-msvideo";
    mime[".bmp"] = "image/bmp";
    mime[".c"] = "text/plain";
    mime[".doc"] = "application/msword";
    mime[".gif"] = "image/gif";
    mime[".gz"] = "application/x-gzip";
    mime[".htm"] = "text/html";
    mime[".ico"] = "image/x-icon";
    mime[".jpg"] = "image/jpeg";
    mime[".png"] = "image/png";
    mime[".txt"] = "text/plain";
    mime[".mp3"] = "audio/mp3";
    mime["default"] = "text/html";
}


std::string MimeType::getMime(const std::string& suffix) {
    pthread_once(&once_control, MimeType::init);
    if(mime.find(suffix) == mime.end()) {
        return mime["default"];
    } else {
        return mime[suffix];
    }
}



HttpData::HttpData(EventLoop* loop, int connfd) 
    : m_loop(loop)
    , m_channel(new Channel(loop, connfd))
    , m_fd(connfd)
    , m_error(false)
    , m_connectionState(H_CONNECTED)
    , m_method(METHOD_GET)
    , m_HTTPVersion(HTTP_11)
    , m_state(STATE_PARSE_URI)
    , m_hState(H_START)
    , m_keepAlive(false){
    m_channel->setReadHandler(std::bind(&HttpData::handleRead, this));
    m_channel->setWriteHandler(std::bind(&HttpData::handleWrite,this));
    m_channel->setConnHandler(std::bind(&HttpData::handleConn, this));
}


void HttpData::reset() {
    m_fileName.clear();
    m_path.clear();
    m_nowReadPos = 0;
    m_state = STATE_PARSE_URI;
    m_hState = H_START;
    m_headers.clear();
    if(m_timer.lock()) {
        std::shared_ptr<TimerNode> my_timer(m_timer.lock());
        my_timer->clearReq();
        m_timer.reset();
    }
}

void HttpData::seperateTimer() {
    if(m_timer.lock()) {
        std::shared_ptr<TimerNode> my_timer(m_timer.lock());
        my_timer->clearReq();
        m_timer.reset();
    }
}

void HttpData::handleClose() {
    m_connectionState = H_DISCONNECTED;
    std::shared_ptr<HttpData> guard(shared_from_this());
    m_loop->removeFromPoller(m_channel);
}

void HttpData::newEvent() {
    m_channel->setEvents(DEFAULT_EVENT);
    m_loop->addToPoller(m_channel, DEFAULT_EXPIRED_TIME);
}

void HttpData::handleRead() {
    __uint32_t& events_ = m_channel->getEvents();
    do {
        bool zero = false;
        int read_num = readn(m_fd, m_inBuffer, zero);
        LOG << "Request:" << m_inBuffer;
        if(m_connectionState == H_DISCONNECTING) {
            m_inBuffer.clear();
            break;
        }
        if(read_num < 0) {
            perror("1");
            m_error = true;
            handleError(m_fd, 400, "Bad Request");
            break;
        }

        else if(zero) {
            //  有请求出现，但是读不到数据，可能是
            //Abrod，或者来自网络的数据没有达到等原因
            //最可能是对端已经关闭了，统一按照对端已经关闭处理
            //error = true;
            m_connectionState = H_DISCONNECTING;
            if(read_num == 0) {
                break;
            }
        }
        if(m_state == STATE_PARSE_URI) {
            URIState flag = this->parseURI();
            if(flag == PARSE_URI_AGAIN) {
                break;
            } else if(flag == PARSE_URI_ERROR) {
                perror("2");
                LOG << "FD= " << m_fd << ", " << m_inBuffer << "parse uri failed****";
                m_inBuffer.clear();
                m_error = true;
                handleError(m_fd, 400, "Bad Request");
                break;
            } else {
                m_state = STATE_PARSE_HEADERS;
            }
        }
        if(m_state == STATE_PARSE_HEADERS) {
            HeaderState flag = this->parseHeaders();
            if(flag == PARSE_URI_AGAIN) {
                break;
            } else if(flag == PARSE_URI_ERROR) {
                perror("3");
                LOG << "FD= " << m_fd <<", " << m_inBuffer << "parse headers failed****";
                m_inBuffer.clear();
                m_error = true;
                handleError(m_fd, 400, "Bad Request");
                break;
            }
            if(m_method == METHOD_POST) {
                m_state = STATE_RECV_BODY;
            } else {
                m_state == STATE_ANALYSIS;
            }
        }
        
        if(m_state == STATE_RECV_BODY) {
            int content_length = -1;
            if(m_headers.find("Content-length") != m_headers.end()) {
                content_length = std::stoi(m_headers["Content-length"]);
            } else {
                m_error = true;
                handleError(m_fd, 400, "Bad Reqeust: Lack of argument(Conten-length)");
                break;
            }
            if(static_cast<int>(m_inBuffer.size()) < content_length) {
                break;
            }
            m_state = STATE_ANALYSIS;
        }
        if(m_state == STATE_ANALYSIS) {
            AnalysisState flag = this->analysisRequest();
            if(flag == ANALYSIS_SUCCESS) {
                m_state = STATE_FINISH;
                break;
            } else {
                m_error = true;
                break;
            }
        }
    } while(false);

    if(!m_error) {
        handleWrite();
    }

    if(!m_error && m_state == STATE_FINISH)  {
        this->reset();
        if(m_inBuffer.size() > 0) {
            if(m_connectionState != H_DISCONNECTING) {
                handleRead();  //inBuffer里面的内容没有解析完，继续调用read解析
            }
        } else if(!m_error && m_connectionState != H_DISCONNECTED) {
            events_ |= EPOLLIN;
        }
    }

}

void HttpData::handleWrite() {
    if(!m_error && m_connectionState != H_DISCONNECTED) {
        __uint32_t& events_ = m_channel->getEvents();
        int size = writen(m_fd, m_outBuffer);
        if(size < 0) {
            perror("writen");
            events_ = 0;
            m_error = true;
        }
        if(m_outBuffer.size() > 0) {
            events_ |= EPOLLOUT;
        }
    }
}

void HttpData::handleConn() {
    seperateTimer();
    __uint32_t& events_ = m_channel->getEvents();
    if(!m_error && m_connectionState == H_CONNECTED) {
        if(events_ != 0) {
            int timeout = DEFAULT_EXPIRED_TIME;
            if(m_keepAlive) {
                timeout = DEFAULT_KEEP_ALIVE_TIME; 
            }
            if((events_ & EPOLLIN) && (events_ & EPOLLOUT)) {
                events_ = __uint32_t(0);
                events_ |= EPOLLOUT; //epollout是从不可写到可写转变时触发，
            }
            events_ |= EPOLLET;
            m_loop->updatePoller(m_channel, timeout);
        
        } else if(m_keepAlive) {
            events_ |= (EPOLLIN | EPOLLET);
            int timeout = DEFAULT_KEEP_ALIVE_TIME;
            m_loop->updatePoller(m_channel, timeout);
        } else {
            events_ |= (EPOLLIN | EPOLLET);
            int timeout = (DEFAULT_KEEP_ALIVE_TIME >> 1);
            m_loop->updatePoller(m_channel, timeout);
        }
    }else if(!m_error && m_connectionState == H_DISCONNECTING
                    && (events_ & EPOLLOUT)) {
        events_ = (EPOLLOUT | EPOLLET);
    } else {
        m_loop->runInLoop(std::bind(&HttpData::handleClose, shared_from_this()));
    }
}

void HttpData::handleError(int fd, int err_num, 
                    std::string short_msg) {
    short_msg = " " + short_msg;
    char send_buff[4096];
    std::string body_buff, header_buff;
    body_buff += "<html><titl>出错了</title>";
    body_buff += "<body bgcolor=\"ffffff\">";
    body_buff += std::to_string(err_num) + short_msg;
    body_buff += "<hr><em> QinLiang web Server</em>\n</body></html>";

    header_buff += "HTTP/1.1" + std::to_string(err_num) + short_msg + "\r\n";
    header_buff += "Content-type: text/html\r\n"; 
    header_buff += "Connection: Close\r\n";
    header_buff += "Content-Length: " + std::to_string(body_buff.size()) + "\r\n";
    header_buff += "Server: QinLiang web Server\r\n";
    header_buff += "\r\n";
    //错误处理不考虑writen不完的情况；
    sprintf(send_buff, "%s", header_buff.c_str());
    writen(fd, send_buff, strlen(send_buff));
    sprintf(send_buff, "%s", body_buff.c_str());
    writen(fd, send_buff, strlen(send_buff));
}

URIState HttpData::parseURI() {
    std::string& str = m_inBuffer;
    std::string cop = str;
    //读到完整的请求号再开始解析请求
    size_t pos = str.find('\r', m_nowReadPos);
    if(pos < 0) {
        return PARSE_URI_AGAIN;
    }
    //去掉请求行所占的空间， 节省空间
    std::string request_line = str.substr(0, pos);
    if(str.size() > pos + 1) { //这里说明除了有请求行的数据，还有headers，body等数据需要处理
        str = str.substr(pos + 1);//请求行数据已经保存到了request_line里面了，str就可以将请求行数据截断了，
    } else {
        str.clear();
    }
    //Method
    int posGet = request_line.find("GET");
    int posPost = request_line.find("POST");
    int posHead = request_line.find("HEAD");
    if(posGet >= 0) {
        pos = posGet;
        m_method = METHOD_GET;
    } else if(posPost >= 0) {
        pos = posPost;
        m_method = METHOD_POST;
    } else if(posHead >= 0){
        pos = posHead;
        m_method = METHOD_HEAD;
    } else {
        return PARSE_URI_ERROR;
    }

    //请求行包含内容：请求方法，url，协议版本
    //如：GET /root/index.html HTTP/1.1

    //filename
    pos = request_line.find("/", pos);
    if(pos < 0) {
        m_fileName = "index.html";
        m_HTTPVersion = HTTP_11;
        return PARSE_URI_SUCCESS;
    } else {
        size_t _pos = request_line.find(" ", pos);
        if(_pos < 0) {
            return PARSE_URI_ERROR;
        } else {
            if(_pos - pos > 1) {
                m_fileName = request_line.substr(pos + 1, _pos - pos -1);
                size_t __pos = m_fileName.find('?');
                if(__pos >= 0) {
                    m_fileName = m_fileName.substr(0, __pos);//去掉路径参数，
                }
            } else {
                m_fileName = "index.html";
            }
        }
        pos = _pos;
    }
    //HTTP 版本号
    pos = request_line.find("/", pos);
    if(pos < 0) {
        return PARSE_URI_ERROR;
    } else {
        if(request_line.size() - pos <= 3) {
            return PARSE_URI_ERROR;
        } else {
            std::string ver = request_line.substr(pos + 1, 3);
            if(ver == "1.0") {
                m_HTTPVersion = HTTP_10;
            } else if(ver == "1.1") {
                m_HTTPVersion = HTTP_11;
            } else {
                return PARSE_URI_ERROR;
            }
        }
    }
    return PARSE_URI_SUCCESS;
}

HeaderState HttpData::parseHeaders() {
    std::string& str = m_inBuffer; //请求行已经处理完成
    int key_start = -1, key_end = -1, vaule_start = -1, value_end = -1;
    int now_read_line_begin = 0;
    bool notFinish = true;
    size_t i = 0;
    for(; i < str.size() && notFinish; ++i) {
        switch (m_hState) {
            case H_START: {
                if(str[i] == '\r' || str[i] == '\n'){
                    break;
                }
                m_hState = H_KEY;
                key_start = i;
                now_read_line_begin = i;
                break;
            }
            case H_KEY: {
                if(str[i] == ':') { //一直循环到':',key-start和key-end之间的字符串就是key了，
                    key_end = i;
                    if(key_end - key_start <= 0) {
                        return PARSE_HEADER_ERROR;
                    }
                    m_hState = H_COLON;
                } else if(str[i] == '\n' || str[i] == '\r') {
                    return PARSE_HEADER_ERROR;
                }
                break;
            }
            case H_COLON: {
                if(str[i] == ' ') {
                    m_hState = H_SPACES_AFTER_COLON;
                } else {
                    return PARSE_HEADER_ERROR;
                }
                break;
            }
            case H_SPACES_AFTER_COLON :{
                m_state == H_VALUE;
                vaule_start = i;
                break;
            }
            case H_VALUE :{
                if(str[i] == '\r') {  //一直循环到i指向'\r',value_start和value_end之间的字符就是value了，
                    m_hState = H_CR;
                    value_end = i;
                    if(value_end - vaule_start <= 0) {
                        return PARSE_HEADER_ERROR;
                    }
                } else if(i - vaule_start > 255) {
                    return PARSE_HEADER_ERROR;
                }
                break;
            }
            case H_CR : {
                if(str[i] == '\n') {
                    m_hState = H_LF;
                    std::string key(str.begin() + key_start, str.begin() + key_end);
                    std::string value(str.begin() + vaule_start, str.begin() + value_end);
                    m_headers[key] = value;
                    now_read_line_begin = i;
                } else {
                    return PARSE_HEADER_ERROR;
                }
                break;
            }
            case H_LF: {
                if(str[i] == '\r') {
                    m_hState = H_END_CR;
                } else {
                    key_start = i;
                    m_hState = H_KEY;
                }
                break;
            }
            case H_END_CR :{
                if(str[i] == '\n') {
                    m_hState = H_END_LF;
                } else {
                    return PARSE_HEADER_ERROR;
                }
                break;
            }
            case H_END_LF :{
                notFinish = false;
                key_start = i;
                now_read_line_begin = i;
                break;
            }
        }
    }
    if(m_hState == H_END_LF) {
        str = str.substr(i); //header处理完后，str截断header部分,
        return PARSE_HEADER_SUCCESS;
    }
    str = str.substr(now_read_line_begin);
    return PARSE_HEADER_AGAIN;
}

AnalysisState HttpData::analysisRequest() {
    if(m_method == METHOD_POST) {

    } else if(m_method == METHOD_GET 
                    || m_method == METHOD_HEAD) {
        std::string header;
        header += "HTTP/1.1 200 OK\r\n";
        if(m_headers.find("Connection") != m_headers.end()
            && (m_headers["Connection"] == "Keep-Alive" ||
                m_headers["Connection"] == "Keep-alive")) {
            m_keepAlive = true;
            header += std::string("Connection: Keep-Alive")
                + "Keep-Alive: timeout="
                + std::to_string(DEFAULT_KEEP_ALIVE_TIME)
                + "\r\n";
        }
        int dot_pos = m_fileName.find('.');
        std::string filetype;   //请求的文件的后缀，
        if(dot_pos < 0) {
            filetype = MimeType::getMime("default");
        } else {
            filetype = MimeType::getMime(m_fileName.substr(dot_pos));
        }
        //echo test
        if(m_fileName == "hello") {
            m_outBuffer =
                "HTTP/1.1 200 OK\r\nContent-type: text/plain\r\n\r\nHello World";
            return ANALYSIS_ERROR;
        }
        if(m_fileName == "favicon.ico") {
            header += "Content-Type: image/png\r\n";
            header += "Content-Length: " + std::to_string(sizeof favicon) + "\r\n"; 
            header += "Server: QinLiang web Server\r\n";
            header += "\r\n";
            m_outBuffer += header;
            m_outBuffer += std::string(favicon, favicon + sizeof favicon);
            return ANALYSIS_SUCCESS;
        }
        
        struct stat sbuf;
        if(stat(m_fileName.c_str(), &sbuf) < 0) {
            header.clear();
            handleError(m_fd, 404, "Not Fount!");
            return ANALYSIS_ERROR;
        }
        header += "Content-Type: image/png\r\n";
        header += "Content-Length: " + std::to_string(sizeof favicon) + "\r\n"; 
        header += "Server: QinLiang web Server\r\n";
        //头部结束
        header += "\r\n";
        m_outBuffer += header;
        
        if(m_method == METHOD_HEAD) {
            return ANALYSIS_SUCCESS;
        }
        int src_fd = open(m_fileName.c_str(), O_RDONLY, 0);
        if(src_fd < 0) {
            m_outBuffer.clear();
            handleError(m_fd, 404, "Not Found!");
            return ANALYSIS_ERROR;
        }
        void* mmapRet = mmap(nullptr, sbuf.st_size, PROT_READ, MAP_PRIVATE, src_fd, 0);
        close(src_fd);
        if(mmapRet == (void*)-1) {
            munmap(mmapRet, sbuf.st_size);
            m_outBuffer.clear();
            handleError(m_fd, 404, "Not Found!");
            return ANALYSIS_ERROR;
        }
        char* src_addr = static_cast<char*>(mmapRet);
        m_outBuffer += std::string(src_addr, src_addr + sbuf.st_size);
        munmap(mmapRet, sbuf.st_size);
        return ANALYSIS_SUCCESS;
    }
    return ANALYSIS_ERROR;
}

