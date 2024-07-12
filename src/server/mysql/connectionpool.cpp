#include "connectionpool.hpp"
#include "public.hpp"

ConnectionPool::ConnectionPool(){
    // 创建初始连接
    for(int i = 0; i < _initSize; i++){
        Connection* conn = new Connection();
        conn->connect(_ip, _user, _password, _dbname);
        _connQue.push(conn);
        _connCnt++;
    }
    sem_init(&_sem, 0, 0);
}

ConnectionPool::~ConnectionPool(){
    closeConnectionPool();
    cout << "数据库连接池销毁..." << endl;
}

void ConnectionPool::closeConnectionPool(){
    // 释放数据库连接
    while(!_connQue.empty()){
        Connection* conn = _connQue.front();
        _connQue.pop();
        delete conn;
    }
    _connCnt = 0;
    sem_destroy(&_sem);
}

ConnectionPool* ConnectionPool::getInstance(){
    // 通过创建静态局部对象实现线程安全
    static ConnectionPool pool;
    return &pool;
}

shared_ptr<Connection> ConnectionPool::getConnction(){
    // 对连接队列加锁
    unique_lock<mutex> lock(_queMutex);
    // 如果队列为空则消费者释放锁并等待其他线程释放连接
    while(_connQue.empty()){
        LOG("get mysql connection timeout...");
        sem_wait(&_sem);
    }
    // 自定义智能指针的析构函数, 使得创建的connection资源不会被回收, 而是重新push到队列中
    shared_ptr<Connection> conn(_connQue.front(), [&](Connection* ptrconn){
        unique_lock<mutex> lock(_queMutex);
        _connQue.push(ptrconn);
        sem_post(&_sem);
    });
    _connQue.pop();
    return conn;
}