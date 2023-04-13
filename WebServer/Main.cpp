#include <getopt.h>
#include <string>
#include "EventLoop.h"
#include "Server.h"
#include "base/Logging.h"

int main(int argc, char** argv) {
    int threadNum = 4;
    int port = 80;
    std::string logPath = "./WebServer.log";

    //parse args
    int opt;
    const char* str = "t:l:p";
    while((opt = getopt(argc, argv, str)) != -1) {
        switch(opt) {
            case 't': {
                threadNum = atoi(optarg);
                break;
            }
            case 'l': {
                logPath = optarg;
                if(logPath.size() < 2 || optarg[0] != '/') {
                    printf("logPath should start with \"/\"\n");
                    abort();
                }
                break;
            }
            case 'p': {
                port = atoi(optarg);
                break;
            }
            default:
                break;
        }
    }

    Logger::setLogFileName(logPath);
#ifndef _PTHREADS
    LOG << "_PTHREADS is not defined!";
#endif
    std::cout << "--------1" << std::endl;
    EventLoop mainLoop;
    std::cout << "--------2" << std::endl;
    Server myHTTPServer(&mainLoop, threadNum, port);
    std::cout << "--------3" << std::endl;
    myHTTPServer.start();
    std::cout << "--------4" << std::endl;
    mainLoop.loop();   //在这里，server才开始监听客户端的请求。
    std::cout << "--------5" << std::endl;
    return 0;
}