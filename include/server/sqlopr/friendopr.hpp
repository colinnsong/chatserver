#ifndef FRIEND_H
#define FRIEND_H

#include "user.hpp"
#include <iostream>
#include <vector>
using namespace std;

// friend数据表的操作类
class FriendOpr{
public:
    // 添加好友关系
    void insert(int userid, int friendid);
    // 多表联合(user表+friend表)查询返回用户好友信息
    vector<User> query(int userid);
};

#endif