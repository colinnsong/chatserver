#include "groupopr.hpp"
#include "connectionpool.hpp"

// 创建群组
bool GroupOpr::createGroup(Group &group){
    // 组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "insert into allgroup(groupname, groupdesc) values('%s', '%s')",
             group.getName().c_str(), group.getDesc().c_str());
    shared_ptr<Connection> mysql = ConnectionPool::getInstance()->getConnction();
    if(mysql->update(sql)){
        // 获取新插入的群组数据的id主键
        group.setId(mysql_insert_id(mysql->getConnection()));
        return true;
    }
    return false;
}

// 加入群组
void GroupOpr::addGroup(int userid, int groupid, string role){
    // 组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "insert into groupuser values(%d, %d, '%s')", groupid, userid, role.c_str());
    shared_ptr<Connection> mysql = ConnectionPool::getInstance()->getConnction();
    mysql->update(sql);
}

// 查询用户所在群组信息
vector<Group> GroupOpr::queryGroups(int userid){
    // 组装sql语句
    char sql[1024] = {0};
    // 多表联查
    sprintf(sql, "select allgroup.id, allgroup.groupname, allgroup.groupdesc from allgroup \
    inner join groupuser on groupuser.groupid = allgroup.id where userid = %d", userid);
    shared_ptr<Connection> mysql = ConnectionPool::getInstance()->getConnction();
    vector<Group> groups;
    MYSQL_RES* res = mysql->query(sql);
    if(res != nullptr){
        MYSQL_ROW row;
        while((row = mysql_fetch_row(res)) != nullptr){
            Group group;
            group.setId(atoi(row[0]));
            group.setName(row[1]);
            group.setDesc(row[2]);
            groups.push_back(group);
        }
        mysql_free_result(res);
    }
    // 查询所有群组内的用户信息
    for(Group& group : groups){
        // 组装sql语句
        char sql[1024] = {0};
        // 多表联查
        sprintf(sql,"select user.id, user.name, user.state, groupuser.grouprole \
        from user, groupuser where user.id = groupuser.userid and groupuser.groupid = %d", group.getId());
        shared_ptr<Connection> mysql = ConnectionPool::getInstance()->getConnction();
        MYSQL_RES* res = mysql->query(sql);
        if(res != nullptr){
            MYSQL_ROW row;
            while((row = mysql_fetch_row(res)) != nullptr){
                GroupUser groupuser;
                groupuser.setId(atoi(row[0]));
                groupuser.setName(row[1]);
                groupuser.setState(row[2]);
                groupuser.setRole(row[3]);
                group.getUsers().push_back(groupuser);
            }
            mysql_free_result(res);
        }
    }
    return groups;
}

// 根据指定的groupid查询群组其他用户的id列表，用于群聊业务
vector<int> GroupOpr::queryGroupUsers(int userid, int groupid){
    // 组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "select userid from groupuser where groupid = %d and userid != %d", groupid, userid);
    shared_ptr<Connection> mysql = ConnectionPool::getInstance()->getConnction();
    vector<int> idvec;
    MYSQL_RES* res = mysql->query(sql);
    if(res != nullptr){
        MYSQL_ROW row;
        while((row = mysql_fetch_row(res)) != nullptr){
            idvec.push_back(atoi(row[0]));
        }
        mysql_free_result(res);
    }
    return idvec;
}