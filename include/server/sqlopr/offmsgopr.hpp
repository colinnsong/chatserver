#ifndef OFFMSGOPR_H
#define OFFMSGOPR_H

#include <iostream>
#include <string>
#include <vector>
using namespace std;

// offlinemessage数据表的操作类
class OffMsgOpr{
public:
    // 存储用户的离线消息
    void insert(int id, string msg);
    // 删除用户的离线消息
    void remove(int id);
    // 查询用户的离线消息
    vector<string> query(int id);
};

#endif