# ChatServer

## 简介
这是一个基于muduo库实现的跨节点聊天服务器项目：采用模块化设计，实现解耦的业务处理与网络模块；使用nginx实现多个服务节点的负载均衡；使用redis作为缓存数据库并充当服务节点之间通信的消息中间件；基本实现了用户登录注册、添加好友、加入群聊、好友聊天、群组聊天、离线消息存储等功能。

## 使用

### 依赖环境
* `cmake(version >= 2.6)`
* `g++(version >= 4.1)`
* `muduo`
* `redis(version >= 4.0)`
* `hiredis`
* `nginx(version >= 1.9)`

### 创建数据库表
```
mysql -u [user] -p [password]
create database chat;
source chat.sql
```

### 项目拉取和编译
```
git clone https://github.com/colinnsong/chatserver.git  
cd chatserver  
mkdir build  
cd build  
cmake ..  
make  
```

### 启动服务端
```
cd ../bin  
./ChatServer 本机ip 自定义port(如需部署多个服务需修改端口)
```

### 启动客户端
```
cd ../bin  
./ChatClient nginx负载均衡器ip nginx监听port
```
