#ifndef CHATSERVICE_H
#define CHATSERVICE_H

#include "json.hpp"
#include "redis.hpp"
#include "useropr.hpp"
#include "offmsgopr.hpp"
#include "friendopr.hpp"
#include "groupopr.hpp"
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <unordered_map>
#include <functional>
#include <mutex>
using namespace std;
using namespace muduo;
using namespace muduo::net;

using json = nlohmann::json;
using MsgHandler = std::function<void (const TcpConnectionPtr&, json&, Timestamp)>;

// 业务模块主类
class ChatService{
public:
    // 获取单例对象的静态接口函数
    static ChatService* getInstance();
    // 获取消息对象的处理函数Handler
    MsgHandler getHandler(int msgid);
    // 处理登录业务
    void login(const TcpConnectionPtr& conn, json& js, Timestamp time);
    // 处理注册业务
    void reg(const TcpConnectionPtr& conn, json& js, Timestamp time);
    // 注销业务
    void loginout(const TcpConnectionPtr& conn, json& js, Timestamp time);
    // 一对一聊天业务
    void one2oneChat(const TcpConnectionPtr& conn, json& js, Timestamp time);
    // 添加好友业务
    void addFriend(const TcpConnectionPtr& conn, json& js, Timestamp time);
    // 创建群组业务
    void createGroup(const TcpConnectionPtr& conn, json& js, Timestamp time);
    // 加入群组业务
    void addGroup(const TcpConnectionPtr& conn, json& js, Timestamp time);
    // 群聊业务
    void groupChat(const TcpConnectionPtr& conn, json& js, Timestamp time);
    // 从redis消息队列中获取订阅的消息
    void handleRedisSubscribeMessage(int userid, string msg);
    // 处理客户端异常退出业务
    void clientCloseException(const TcpConnectionPtr& conn);
    // 处理服务器异常退出业务
    void reset();
private:
    // 单例设计模式需要将构造函数设置成私有
    ChatService();
    // 存储消息id和其对应的业务处理方法
    unordered_map<int, MsgHandler> _msgHandlerMap;
    // 存储在线用户的通信连接
    unordered_map<int, TcpConnectionPtr> _userConnMap;
    // 互斥锁，保证_userConnectionMap上的线程安全
    mutex _connMutex;

    // user数据操作类对象
    UserOpr _useropr;
    // offlinemessage数据操作类对象
    OffMsgOpr _offmsgopr;
    // friend数据操作类对象
    FriendOpr _friendopr;
    // group数据操作类对象
    GroupOpr _groupopr;

    // redis操作对象
    Redis _redis;
};

#endif