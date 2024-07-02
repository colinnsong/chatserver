#include "chatserver.hpp"
#include "chatservice.hpp"
#include <signal.h>

// 处理服务器的异常中断, 如ctrl+c
void resetHandler(int){
    ChatService::getInstance()->reset();
    exit(0);
}

int main(int argc, char **argv){
    if (argc < 3)
    {
        cerr << "command invalid! example: ./ChatServer 127.0.0.1 6000" << endl;
        exit(-1);
    }
    // 解析通过命令行参数传递的ip和port
    char *ip = argv[1];
    uint16_t port = atoi(argv[2]);
    // 使用signal函数捕捉SIGINT信号(通常是由ctrl+c产生的)
    signal(SIGINT, resetHandler);
    
    EventLoop loop; // epoll
    InetAddress addr(ip, port);
    ChatServer server(&loop, addr, "ChatServer");
    server.start();
    loop.loop(); // epoll_wait()
    return 0;
}