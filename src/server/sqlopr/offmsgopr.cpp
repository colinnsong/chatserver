#include "offmsgopr.hpp"
#include "connectionpool.hpp"

// 存储用户的离线消息
void OffMsgOpr::insert(int id, string msg){
    // 组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "insert into offlinemessage values(%d, '%s')", id, msg.c_str());
    shared_ptr<Connection> mysql = ConnectionPool::getInstance()->getConnction();
    mysql->update(sql);
}

// 删除用户的离线消息
void OffMsgOpr::remove(int id){
    // 组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "delete from offlinemessage where userid = %d", id);
    shared_ptr<Connection> mysql = ConnectionPool::getInstance()->getConnction();
    mysql->update(sql);
}

// 查询用户的离线消息
vector<string> OffMsgOpr::query(int id){
    // 组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "select message from offlinemessage where userid = %d", id);
    shared_ptr<Connection> mysql = ConnectionPool::getInstance()->getConnction();
    vector<string> msg;
    MYSQL_RES* res = mysql->query(sql);
    if(res != nullptr){
        MYSQL_ROW row;
        while((row = mysql_fetch_row(res)) != nullptr){
            msg.push_back(row[0]);
        }
        mysql_free_result(res);
    }
    return msg;
}