#include<muduo/net/EventLoop.h>
#include<muduo/net/TcpServer.h>
#include<muduo/net/InetAddress.h>
#include<muduo/base/Timestamp.h>

#include<unistd.h>

using namespace muduo;
using namespace muduo::net;

void onConnection(const TcpConnectionPtr& conn){
    if(conn->connected()){
        printf("onConnection() :new connection [%s] from %s , %s\n",
            conn->name().c_str(),
            conn->peerAddress().toIpPort().c_str(),
            Timestamp::now().toFormattedString().c_str());
    }
    else {
        printf("onConnection(): new connection %s is close , %s\n",
            conn->name().c_str(),
            Timestamp::now().toFormattedString().c_str());
    }
}


void onMessage(const TcpConnectionPtr& conn,Buffer *,Timestamp timestamp){
    printf("onMessage() :new Message [%s] from %s , %s\n",
        conn->name().c_str(),
        conn->peerAddress().toIpPort().c_str(),
        Timestamp::now().toFormattedString().c_str());
}


int main(){
    printf("main(): pid = %d , now = %s\n",::getpid(),Timestamp::now().toFormattedString().c_str());


    //创建当前线程的eventloop对象，其构造函数会创建一个epoll类，其内部会创建一个epoll文件描述符，用来io复用
    //其他知识：eventloop 会创建一个timerqueue类，进行定时器管理，还有会创建一个文件描述符，方便其他线程将其唤醒
    EventLoop loop;

    //传递端口号，构造一个InetAddress类，里面是sockaddr_in结构体，然后封装了一些函数方便调用
    InetAddress inetaddress(8888);
    
    // 含有主要两个对象，  1.Acceptor主要用于管理监听套接字，处理可读事件，生成文件描述符和对等端地址，并调用这里注册的回调函数
    //                  2.EventLoopThreadPool 主要用于对建立连接之后的Tcpconnection进行管理，为提高性能，较少使用锁
    //                  Eventloop的事件处理会在所属线程进行执行，需要一些使其使用负载均衡的函数
    TcpServer ts(&loop,inetaddress,"testserver");

    //设置回调函数，当期有连接到来的时候,acceptor已经将socket和对等端地址准备好了，用这两个参数来实例化出Tcoconnection对象
    //设置其回调函数，将其注册到对应的Eventloopthread中
    ts.setConnectionCallback(onConnection);
    ts.setMessageCallback(onMessage);

    //调用acceptor的listen,让其将事件注册到epoll中
    ts.start();

    loop.loop();

}