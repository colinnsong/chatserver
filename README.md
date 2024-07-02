# ChatServer

## 简介
这是一个基于muduo库实现的集群聊天服务器项目，使用nginx实现多台服务器的负载均衡，使用redis作为服务器通信的消息中间件

## 使用

### 依赖环境
* `cmake`
* `g++`
* `muduo`
* `redis`
* `nginx`

### 项目拉取和编译
git clone https://github.com/colinnsong/chatserver.git  
cd chatserver  
mkdir build  
cd build  
cmake ..  
make  
