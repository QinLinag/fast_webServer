#ifndef __THREADPOOL_H__
#define __THREADPOOL_H__

#include "Channel.h"
#include <pthread.h>
#include <functional>
#include <memory>
#include <vector>
#include "base/Logging.h"

const int THREADPOOL_INVALID = -1;
const int THREADPOOL_LOCK_FAILURE = -2;
const int THREADPOOL_QUQUE_FULL = -3;
const int THREADPOOL_SHUTDOWN = -4;
const int THREADPOOL_THREAD_FAILURE = -5;
const int THREADPOOL_GRACEFUL = 1;

const int MAX_THREADS = 1024;
const int MAX_QUEUE = 65535;


typedef enum {
    immediate_shutdown = 1,
    graceful_shutdown = 2
} ShutDownOption;

struct ThreadPoolTask {
    std::function<void(std::shared_ptr<void>) > fun;
    std::shared_ptr<void> args;
};



class ThreadPool {
public:
    static int threadpool_create(int _thread_count, int _queue_size);
    static int threadpool_add(std::shared_ptr<void> args, std::function<void(std::shared_ptr<void>) > fun);
    static int threadpool_destroy(ShutDownOption shutdown_option = graceful_shutdown);
    static int threadpool_free();
    static void* threadpool_thread(void* args);

private:
    static pthread_mutex_t lock;
    static pthread_cond_t notify;

    static std::vector<pthread_t> threads;
    static std::vector<ThreadPoolTask> queue;
    static int thread_count;
    static int queue_size;
    static int head;

    static int tail;
    static int count;
    static int shutdown;
    static int started;
};












#endif