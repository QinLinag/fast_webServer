#ifndef __TIMER_H__
#define __TIMER_H__

#include <unistd.h>
#include <deque>
#include <memory>
#include <queue>
#include "HttpData.h"
#include "base/MutexLock.h"
#include "base/noncopyable.h"

class HttpData;

class TimerNode {
public:
    TimerNode(std::shared_ptr<HttpData> requestData, int timeout);
    ~TimerNode();
    TimerNode(TimerNode &tn);
    void update(int timeout);
    bool isValid();
    void clearReq();
    void setDeletd() { m_deleted = true;}
    bool isDeleted() { return m_deleted;}
    size_t getExpTime() { return m_expiredTime;}
private:
    bool m_deleted;
    size_t m_expiredTime;
    std::shared_ptr<HttpData> m_SPHttpData;
};

struct TimerCmp {
    bool operator()(std::shared_ptr<TimerNode>& a,
            std::shared_ptr<TimerNode>& b) const {
        return a->getExpTime() > b->getExpTime();        
    }
};

class TimerManager {
public:
    TimerManager();
    ~TimerManager();
    void addTimer(std::shared_ptr<HttpData> SPHttpData, int timeout);
    void handleExpiredEvent();

private:
    typedef std::shared_ptr<TimerNode> SPTimerNode;
    std::priority_queue<SPTimerNode, std::deque<SPTimerNode>, TimerCmp> timerNodeQueue;
};






#endif