#ifndef MYSQL_H
#define MYSQL_H

#include <mysql/mysql.h>
#include <iostream>
#include <string>
using namespace std;

class MySQL
{
public:
    // 初始化数据库连接
    MySQL();
    // 释放数据库连接资源
    ~MySQL();
    // 连接数据库
    bool connect();
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