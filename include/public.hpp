#ifndef PUBLIC_H
#define PUBLIC_H

#define LOG(str) cout << __FILE__ << ":" << __LINE__ << " " << \
        __TIMESTAMP__ << ":" << str << endl;

enum MsgType{
    LOGIN_MSG = 1, // 登录消息
    LOGIN_MSG_ACK, // 登录响应消息
    LOGINOUT_MSG, // 注销消息
    REG_MSG, // 注册消息
    REG_MSG_ACK, // 注册响应消息
    ONE2ONE_CHAT_MSG, // 一对一聊天消息
    ADD_FRIEND_MSG, // 添加好友消息
    CREATE_GROUP_MSG, // 创建群组消息
    ADD_GROUP_MSG, // 加入群组消息
    GROUP_CHAT_MSG, // 群聊天消息
    REFRESH_MSG, // 刷新消息
    REFRESH_MSG_ACK // 刷新响应消息
};

#endif