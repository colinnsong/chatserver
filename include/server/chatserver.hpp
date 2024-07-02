#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
using namespace muduo;
using namespace muduo::net;

// 网络模块主类
class ChatServer{
public:
    // 初始化服务器对象
    ChatServer(EventLoop* loop, const InetAddress& listenAddr, const string& nameArg);
    // 启动服务器
    void start();
private:
    // 专门处理用户连接和断开事件的回调函数
    void onConnection(const TcpConnectionPtr&);
    // 专门处理用户读写事件的回调函数
    void onMessage(const TcpConnectionPtr&, Buffer*, Timestamp);
    TcpServer _server;
    EventLoop* _loop;
};

#endif