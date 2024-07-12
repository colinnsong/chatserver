#include "connection.hpp"
#include <muduo/base/Logging.h>

// 数据库配置信息
// static string server = "127.0.0.1";
// static string user = "colin";
// static string password = "123456";
// static string dbname = "chat";

// 初始化数据库连接
Connection::Connection(){
    _conn = mysql_init(nullptr);
}

// 释放数据库连接资源
Connection::~Connection(){
    if (_conn != nullptr)
        mysql_close(_conn);
}

// 连接数据库
bool Connection::connect(string server, string user, string password, string dbname)  {
    MYSQL *p = mysql_real_connect(_conn, server.c_str(), user.c_str(),
                                password.c_str(), dbname.c_str(), 3306, nullptr, 0);
    if (p != nullptr){
        LOG_INFO << "connect mysql succeed!";
        mysql_query(_conn, "set names gbk");
    }
    else{
        LOG_INFO << "connect mysql failed!";
    }
    return p;
}

// 更新操作
bool Connection::update(string sql){
    if (mysql_query(_conn, sql.c_str())){
        LOG_INFO << __FILE__ << ":" << __LINE__ << ":" << sql << "update failed!";
        return false;
    }
    return true;
}

// 查询操作
MYSQL_RES* Connection::query(string sql){
    if (mysql_query(_conn, sql.c_str())){
        LOG_INFO << __FILE__ << ":" << __LINE__ << ":" << sql << "read failed!";
        return nullptr;
    }
    return mysql_use_result(_conn);
}

MYSQL* Connection::getConnection(){
    return this->_conn;
}