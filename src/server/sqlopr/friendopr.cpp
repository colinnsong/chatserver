#include "friendopr.hpp"
#include "mysql.hpp"

// 添加好友关系
void FriendOpr::insert(int userid, int friendid){
    // 组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "insert into friend values(%d, %d)", userid, friendid);
    MySQL mysql;
    if(mysql.connect()){
        mysql.update(sql);
    }
}

// 多表联查(user表+friend表)并返回用户好友信息
vector<User> FriendOpr::query(int userid){
    // 组装sql语句
    char sql[1024] = {0};
    // sql = "select user.id, user.name, user.state from user, friend \
    where user.id = friend.friendid and friend.userid = %d"
    sprintf(sql, "select user.id, user.name, user.state from user inner join friend \
    on user.id = friend.friendid where friend.userid = %d", userid);
    MySQL mysql;
    vector<User> friends;
    if(mysql.connect()){
        MYSQL_RES* res = mysql.query(sql);
        if(res != nullptr){
            MYSQL_ROW row;
            while((row = mysql_fetch_row(res)) != nullptr){
                User user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setState(row[2]);
                friends.push_back(user);
            }
            mysql_free_result(res);
        }
    }
    return friends;
}