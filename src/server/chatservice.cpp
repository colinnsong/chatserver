#include "chatservice.hpp"
#include "public.hpp"
#include <muduo/base/Logging.h>
#include <functional>
#include <vector>
#include <string>
using namespace std;
using namespace muduo;

ChatService::ChatService(){
    _msgHandlerMap.insert({LOGIN_MSG, bind(&ChatService::login, this, _1, _2, _3)});
    _msgHandlerMap.insert({REG_MSG, bind(&ChatService::reg, this, _1, _2, _3)});
    _msgHandlerMap.insert({LOGINOUT_MSG, bind(&ChatService::loginout, this, _1, _2, _3)});
    _msgHandlerMap.insert({ONE2ONE_CHAT_MSG, bind(&ChatService::one2oneChat, this, _1, _2, _3)});
    _msgHandlerMap.insert({ADD_FRIEND_MSG, bind(&ChatService::addFriend, this, _1, _2, _3)});
    _msgHandlerMap.insert({CREATE_GROUP_MSG, bind(&ChatService::createGroup, this, _1, _2, _3)});
    _msgHandlerMap.insert({ADD_GROUP_MSG, bind(&ChatService::addGroup, this, _1, _2, _3)});
    _msgHandlerMap.insert({GROUP_CHAT_MSG, bind(&ChatService::groupChat, this, _1, _2, _3)});

    // 连接Redis服务器并设置消息上报回调
    if(_redis.connect()){
        _redis.init_notify_handler(bind(&ChatService::handleRedisSubscribeMessage, this, _1, _2));
    }
}

// 获取单例对象的静态接口函数
ChatService* ChatService::getInstance(){
    // 通过创建静态局部对象实现线程安全
    static ChatService service;
    return &service;
}

// 获取消息对象的处理函数Handler
MsgHandler ChatService::getHandler(int msgid){
    // 如果没有对应的事件处理函数则记录错误日志
    if (_msgHandlerMap.find(msgid) == _msgHandlerMap.end()){
        // 返回一个默认处理器，只执行LOG操作
        return [=](const TcpConnectionPtr& conn, json& js, Timestamp time){
            LOG_ERROR << "msgid:" << msgid << " cannot find handler!";
        };
    }
    else{
        return _msgHandlerMap[msgid];
    }
}

// 处理登录业务
void ChatService::login(const TcpConnectionPtr& conn, json& js, Timestamp time){
    int id = js["id"].get<int>();
    string password = js["password"];
    User user = _useropr.query(id);
    if(user.getId() != -1 && user.getPassword() == password){
        if(user.getState() == "online"){
            // 重复登录
            json response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["errno"] = 1;
            response["errormsg"] = "the user is online!";
            conn->send(response.dump());
        }
        else{
            // 登录成功, 更新用户的状态
            user.setState("online");
            _useropr.updateState(user);

            // 记录用户连接
            {
                lock_guard<mutex> lock(_connMutex);
                _userConnMap.insert({id, conn});
            }

            // 在redis中订阅channel(id)
            _redis.subscribe(id);

            json response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["errno"] = 0;
            response["id"] = id;
            response["name"] = user.getName();

            // 查询用户是否有离线消息
            vector<string> offlinemsg = _offmsgopr.query(id);
            if(!offlinemsg.empty()){
                // 读取用户的离线消息后, 删除该用户的离线消息
                response["offlinemsg"] = offlinemsg;
                _offmsgopr.remove(id);
            }
            
            // 查询用户的好友信息
            vector<User> friends = _friendopr.query(id);
            if(!friends.empty()){
                vector<string> friendmsg;
                for(User& user : friends){
                    json js;
                    js["id"] = user.getId();
                    js["name"] = user.getName();
                    js["state"] = user.getState();
                    friendmsg.push_back(js.dump());
                }
                response["friends"] = friendmsg;
            }

            // 查询用户的群组信息
            vector<Group> groups = _groupopr.queryGroups(id);
            if(!groups.empty()){
                vector<string> groupmsg;
                for(Group& group : groups){
                    json js;
                    js["id"] = group.getId();
                    js["groupname"] = group.getName();
                    js["groupdesc"] = group.getDesc();
                    vector<string> usermsg;
                    vector<GroupUser> groupusers = group.getUsers();
                    for(GroupUser& user : groupusers){
                        json userjs;
                        userjs["id"] = user.getId();
                        userjs["name"] = user.getName();
                        userjs["state"] = user.getState();
                        userjs["role"] = user.getRole();
                        usermsg.push_back(userjs.dump());
                    }
                    js["users"] = usermsg;
                    groupmsg.push_back(js.dump());
                }
                response["groups"] = groupmsg;
            }
            conn->send(response.dump());
        }
    }
    else{
        if(user.getId() == -1){
            // 用户名不存在
            json response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["errno"] = 1;
            response["errmsg"] = "the user is not exit!";
            conn->send(response.dump());
        }
        else{
            // 密码错误
            json response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["errno"] = 1;
            response["errmsg"] = "the password is wrong!";
            conn->send(response.dump());
        }
    }
}

// 注销业务
void ChatService::loginout(const TcpConnectionPtr& conn, json& js, Timestamp time){
    int userid = js["id"].get<int>();
    User user;
    {
        lock_guard<mutex> lock(_connMutex);
        // 从_userConnectionMap中删除连接信息
        for(auto it = _userConnMap.begin(); it != _userConnMap.end(); it++){
            if(it->first == userid){
                user.setId(userid);
                _userConnMap.erase(it);
                break;
            }
        }
    }
    // 在redis中取消订阅channel(id)
    _redis.unsubscribe(userid);

    if(user.getId() != -1){
        // 更新用户状态
        user.setState("offline");
        _useropr.updateState(user);
    }

}

// 处理注册业务
void ChatService::reg(const TcpConnectionPtr& conn, json& js, Timestamp time){
    string name = js["name"];
    string password = js["password"];
    User user;
    user.setName(name);
    user.setPassword(password);
    bool state = _useropr.insert(user);
    if(state){
        // 注册成功
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 0;
        response["id"] = user.getId();
        response["name"] = user.getName();
        conn->send(response.dump());
    }
    else{
        // 注册失败
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 1;
        response["errormsg"] = "registration failed!";
        conn->send(response.dump());
    }
}

/*
    一对一聊天业务
    msgid: 业务id
    id: 发送方用户id
    name: 发送方用户名
    to: 接收方用户id
    msg: 消息内容
*/
void ChatService::one2oneChat(const TcpConnectionPtr& conn, json& js, Timestamp time){
    int toid = js["toid"].get<int>();
    {
        lock_guard<mutex> lock(_connMutex);
        auto it = _userConnMap.find(toid);
        if(it != _userConnMap.end()){
            // 目标用户在同一台服务器上且在线, 转发消息
            auto toconn = it->second;
            toconn->send(js.dump());
            return;
        }
    }
    // 查询toid用户是否在线,如果在线则在其他服务器上登录
    User user = _useropr.query(toid);
    if(user.getState() == "online"){
        _redis.publish(toid, js.dump());
        return;
    }
    // 目标用户不在线, 存储离线消息
    _offmsgopr.insert(toid, js.dump());
}

// 添加好友业务
void ChatService::addFriend(const TcpConnectionPtr& conn, json& js, Timestamp time){
    int userid = js["id"].get<int>();
    int friendid = js["friendid"].get<int>();
    // 存储好友id
    _friendopr.insert(userid, friendid);
}

// 创建群组业务
void ChatService::createGroup(const TcpConnectionPtr& conn, json& js, Timestamp time){
    int creatorid = js["id"].get<int>();
    string name = js["groupname"];
    string desc = js["groupdesc"];
    // 存储新群组信息
    Group group;
    group.setName(name);
    group.setDesc(desc);
    if(_groupopr.createGroup(group)){
        // 将创建者加入群组
        _groupopr.addGroup(creatorid, group.getId(), "creator");
    }
}

// 加入群组业务
void ChatService::addGroup(const TcpConnectionPtr& conn, json& js, Timestamp time){
    int userid = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();
    _groupopr.addGroup(userid, groupid, "normal");
}

// 群聊业务
void ChatService::groupChat(const TcpConnectionPtr& conn, json& js, Timestamp time){
    int userid = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();
    vector<int> groupuserid = _groupopr.queryGroupUsers(userid, groupid);
    lock_guard<mutex> lock(_connMutex);
    for(int id : groupuserid){
        auto it = _userConnMap.find(id);
        if(it != _userConnMap.end()){
            // 如果群组用户在同一台服务器上且在线
            auto toconn = it->second;
            toconn->send(js.dump());
        }
        else{
            // 查询toid用户是否在线,如果在线则在其他服务器上登录
            User user = _useropr.query(id);
            if(user.getState() == "online"){
                _redis.publish(id, js.dump());
            }
            else{
                _offmsgopr.insert(id, js.dump());
            }
        }
    }
}

// 从redis消息队列中获取订阅的消息
void ChatService::handleRedisSubscribeMessage(int userid, string msg){
    lock_guard<mutex> lock(_connMutex);
    auto it = _userConnMap.find(userid);
    if (it != _userConnMap.end())
    {
        it->second->send(msg);
        return;
    }
    // 如果从redis获取消息的时候用户下线了,则存储离线消息
    _offmsgopr.insert(userid, msg);
}

// 处理客户端异常退出业务
void ChatService::clientCloseException(const TcpConnectionPtr& conn){
    User user;
    {
        lock_guard<mutex> lock(_connMutex);
        // 从_userConnectionMap中删除连接信息
        for(auto it = _userConnMap.begin(); it != _userConnMap.end(); it++){
            if(it->second == conn){
                user.setId(it->first);
                _userConnMap.erase(it);
                break;
            }
        }
    }
    // 在redis中取消订阅channel(id)
    _redis.unsubscribe(user.getId());

    if(user.getId() != -1){
        // 更新用户状态
        user.setState("offline");
        _useropr.updateState(user);
    }
}

// 处理服务器异常退出业务
void ChatService::reset(){
    // 重置用户状态
    _useropr.resetState();
}