#include "useropr.hpp"
#include "mysql.hpp"
#include <iostream>
using namespace std;

// 添加user数据
bool UserOpr::insert(User &user){
    // 组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "insert into user(name, password, state) values('%s', '%s', '%s')",
             user.getName().c_str(), user.getPassword().c_str(), user.getState().c_str());
    MySQL mysql;
    if(mysql.connect()){
        if(mysql.update(sql)){
            // 获取新插入的用户数据的id主键
            user.setId(mysql_insert_id(mysql.getConnection()));
            return true;
        }
    }
    return false;
}

// 查询user数据
User UserOpr::query(int id){
    // 组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "select * from user where id = %d", id);
    MySQL mysql;
    if(mysql.connect()){
        MYSQL_RES* res = mysql.query(sql);
        if(res != nullptr){
            MYSQL_ROW row = mysql_fetch_row(res);
            if(row != nullptr){
                User user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setPassword(row[2]);
                user.setState(row[3]);
                // 释放mysql.h内申请的动态资源
                mysql_free_result(res);
                return user;
            }
        }
    }
    return User();
}

// 修改user数据
bool UserOpr::updateState(User user){
    // 组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "update user set state = '%s' where id = %d", 
            user.getState().c_str(), user.getId());
    MySQL mysql;
    if(mysql.connect() && mysql.update(sql)){
        return true;
    }
    return false;
}

// 重置user状态
void UserOpr::resetState(){
    // 组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "update user set state = 'offline' where state ='online'");
    MySQL mysql;
    if(mysql.connect()){
        mysql.update(sql);
    }
    
}