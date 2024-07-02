#ifndef USEROPR_H
#define USEROPR_H

#include "user.hpp"

// user表数据的操作类
class UserOpr{
public:
    // 添加user数据
    bool insert(User &user);
    // 查询user数据
    User query(int id);
    // 修改user数据
    bool updateState(User user);
    // 重置user状态
    void resetState();
};

#endif