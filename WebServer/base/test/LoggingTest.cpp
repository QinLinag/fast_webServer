#include "../Logging.h"
#include "../Thread.h"
#include <string>
#include <unistd.h>
#include <vector>
#include <memory>
#include <iostream>

void threadFunc() {
    for(int i = 0; i < 100000; ++i) {
        LOG << i;
    }
}

void type_test() {
    std::cout << "--------type test---------" << std::endl;
    LOG << 0;
    LOG << 1234567890123;
    LOG << 1.0f;
    LOG << 3.1415926;
    LOG << (short)1;
    LOG << (long long)1;
    LOG << (unsigned int)1;
    LOG << (unsigned long)1;
    LOG << (long double)1.6555556;
    LOG << (unsigned long long)1;
    LOG << 'c';
    LOG << "abcdeefg";
    LOG << std::string("This is a string");
}

void stressing_single_thread() {
    std::cout << "------stressing test single thread----------" << std::endl;
    for(int i = 0; i < 100000; ++i){
        LOG << i;
    }
}

void stressing_multi_threas(int threadNum = 4) {
    std::cout << "------stressing test multi thread----------" << std::endl;
    std::vector<std::shared_ptr<Thread> > vsp;
    for(int i = 0; i < threadNum; ++i) {
        std::shared_ptr<Thread> tmp(new Thread(threadFunc, "threadfunc"));
        vsp.push_back(tmp);
    } 
    for(int i = 0; i < threadNum; ++i) {
        vsp[i]->start();
    }
    sleep(3);
}

void other() {
    std::cout << "------other test----------" << std::endl;
    LOG << "fddsa" << 'c' << 0 << 3.666 << std::string("This is string");
}


int main(int argc, char** argv){
    type_test();
    sleep(3);

    stressing_single_thread();
    sleep(3);

    other();
    sleep(3);

    stressing_multi_threas();
    sleep(3);
    return 0;
}
