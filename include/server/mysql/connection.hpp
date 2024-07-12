#ifndef MYSQL_H
#define MYSQL_H

#include <mysql/mysql.h>
#include <iostream>
#include <string>
using namespace std;

class Connection
{
public:
    // 初始化数据库连接
    Connection();
    // 释放数据库连接资源
    ~Connection();
    // 连接数据库
    bool connect(string, string, string, string);
    // 更新操作
    bool update(string sql);
    // 查询操作
    MYSQL_RES* query(string sql);
    // 获取当前连接
    MYSQL* getConnection();
private:
    MYSQL *_conn;
};

#endif