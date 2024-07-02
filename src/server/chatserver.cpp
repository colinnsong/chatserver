#include "chatserver.hpp"
#include "chatservice.hpp"
#include "json.hpp"
#include <iostream>
#include <string>
#include <functional>
using namespace std;
using json = nlohmann::json;

ChatServer::ChatServer(EventLoop* loop, const InetAddress& listenAddr, const string& nameArg): 
        _server(loop, listenAddr, nameArg), _loop(loop){
    /*
        给服务器注册回调函数：
        成员函数自带一个this指针参数，这和set_Callback()
        需要传入的函数对象参数数量不一致，因此需要使用bind
        创建一个适配传入参数数量一致的函数对象。
    */
    // 注册用户连接和断开事件的回调函数
    _server.setConnectionCallback(bind(&ChatServer::onConnection, this, _1));
    // 注册用户读写事件回调函数
    _server.setMessageCallback(bind(&ChatServer::onMessage, this, _1, _2, _3));
    // 设置线程数量：1个I/O线程，3个工作线程
    _server.setThreadNum(4); 
}

void ChatServer::start(){
    _server.start();
}

// 专门处理用户的连接和断开事件
void ChatServer::onConnection(const TcpConnectionPtr& conn){
    // 连接成功
    if(conn->connected()){
        cout << conn->peerAddress().toIpPort() << " connected..." << endl;
    }
    // 连接断开
    else{
        ChatService::getInstance()->clientCloseException(conn);
        cout << conn->peerAddress().toIpPort() << " disconnected..." << endl;
        conn->shutdown(); // close()
    }
}

// 专门处理用户的读写事件
void ChatServer::onMessage(const TcpConnectionPtr& conn, Buffer* buffer, Timestamp time){
    // 拿到服务器接收缓冲区中的json数据并转换成string
    string buf = buffer->retrieveAllAsString();
    // 数据的反序列化
    json js = json::parse(buf);
    // 网络模块和业务模块需要完全解耦
    auto msgHandler = ChatService::getInstance()->getHandler(js["msgid"].get<int>());
    msgHandler(conn, js, time);
}