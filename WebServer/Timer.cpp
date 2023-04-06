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
    
}

TimerNode::~TimerNode() {

}
TimerNode::TimerNode() {

}
TimerNode::TimerNode(TimerNode &tn) {

}

void TimerNode::update(int timeout) {

}

bool TimerNode::isValid() {

}

void TimerNode::clearReq() {

}



TimerManager();
~TimerManager();
void addTimer(std::shared_ptr<HttpData> SPHttpData, int timeout);
void handleExpiredEvent();