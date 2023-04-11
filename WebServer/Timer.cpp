#include "Timer.h"
#include <sys/time.h>
#include <unistd.h>
#include <queue>




TimerNode::TimerNode(std::shared_ptr<HttpData> requestData, int timeout) 
    : m_deleted(false)
    , m_SPHttpData(requestData){
    struct timeval now;
    gettimeofday(&now, nullptr);
    //以毫秒计数
    m_expiredTime = 
        (((now.tv_sec % 10000) * 1000) + (now.tv_usec / 1000)) + timeout;
}

TimerNode::~TimerNode() {
    if(m_SPHttpData) {
        m_SPHttpData->handleClose();
    }
}

TimerNode::TimerNode(TimerNode &tn) 
    : m_SPHttpData(tn.m_SPHttpData)
    , m_expiredTime(0){
}

void TimerNode::update(int timeout) {
    struct timeval now;
    gettimeofday(&now, nullptr);
    m_expiredTime = 
        (((now.tv_sec % 10000) * 1000) + (now.tv_usec / 1000)) + timeout;
}

bool TimerNode::isValid() {
    struct timeval now;
    gettimeofday(&now, nullptr);
    size_t tempTime = 
        (((now.tv_sec % 10000) * 1000) + (now.tv_usec / 1000));
    if(tempTime < m_expiredTime) {
        return true;
    } else {
        this->setDeletd();
        return false;
    }
}

void TimerNode::clearReq() {
    m_SPHttpData.reset();
    this->setDeletd();
}



TimerManager::TimerManager() {
}

TimerManager::~TimerManager() {
}

void TimerManager::addTimer(std::shared_ptr<HttpData> SPHttpData, 
                            int timeout) {
    SPTimerNode new_node(new TimerNode(SPHttpData, timeout));
    timerNodeQueue.push(new_node);
    SPHttpData->linkTimer(new_node);
}

void TimerManager::handleExpiredEvent() {
    while(!timerNodeQueue.empty()) {
        SPTimerNode ptimer_now = timerNodeQueue.top();
        if(!ptimer_now || ptimer_now->isDeleted()) {
            timerNodeQueue.pop();
        } else if(!ptimer_now->isValid()) {
            ptimer_now->setDeletd();
            timerNodeQueue.pop();
        } else {  //队列通过expireTime排序了的，如果最前面时间没有超过，那么后面的肯定没有超过，所以这里就直接break出while
            break;
        }
    }
}





