#ifndef GROUPOPR_H
#define GROUPOPR_H

#include "group.hpp"
#include <string>
#include <vector>
using namespace std;

// 群组相关表(allgroup,groupuser)的操作类
class GroupOpr{
public:
    // 创建群组
    bool createGroup(Group &group);
    // 加入群组
    void addGroup(int userid, int groupid, string role);
    // 查询用户所在群组信息
    vector<Group> queryGroups(int userid);
    // 根据指定的groupid查询群组其他用户的id列表，用于群聊业务
    vector<int> queryGroupUsers(int userid, int groupid);
};

#endif