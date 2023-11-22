#include<muduo/net/TcpServer.h>

#include<muduo/base/Logging.h>
#include<muduo/base/Thread.h>
#include<muduo/net/EventLoop.h>
#include<muduo/net/InetAddress.h>
#include<muduo/base/Timestamp.h>

#include<utility>

#include<stdio.h>
#include<unistd.h>


using namespace muduo;
using namespace muduo::net;


class EchoServer
{
public:
    EchoServer(EventLoop* loop,InetAddress inetaddress,int threadNum):loop_(loop),tcpserver_(loop_,inetaddress,"echoserver"){
        tcpserver_.setConnectionCallback([&](const TcpConnectionPtr& conn){this->onConnection(conn);});
        tcpserver_.setMessageCallback([&](const TcpConnectionPtr& conn, Buffer* buf, Timestamp time){
             this->onMessage(conn,buf,time);});
        tcpserver_.setThreadNum(threadNum);
    }

    void start(){
        tcpserver_.start();
    }
private:

    void onConnection(const TcpConnectionPtr& conn){
        if(conn->connected()){
            LOG_INFO<<"onConnection() :new connection ["<<conn->name().c_str()<<"] from "
                <<conn->peerAddress().toIpPort().c_str();
            conn->send("hello!\n");
        }
        else {
            LOG_INFO<<"onConnection() :connection ["<<conn->name().c_str()<<"] is close "
                <<conn->peerAddress().toIpPort().c_str();
        }
    }

    void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp time){
        string msg(buf->retrieveAllAsString());
        LOG_TRACE << conn->name() << " recv " << msg.size() << " bytes at " << time.toString();
        LOG_INFO << "onMessage() : tid = "<<CurrentThread::tid();
        if (msg == "exit\r\n")
        {
        conn->send("bye\r\n");
        conn->shutdown();
        }
        if (msg == "quit\r\n")
        {
        loop_->quit();
        }
        conn->send(msg);
        //LOG_INFO<<msg<<" "<<msg.size();
    }

    EventLoop* loop_;
    TcpServer tcpserver_;
};


int main(int argc,char * argv[]){
    LOG_INFO<<"main() pid = "<<::getpid()<<" ,tid = "<<CurrentThread::tid();

    int n=0;
    if(argc>1){
        n=atoi(argv[1]);
    }

    EventLoop loop;

    InetAddress listen(8888);


    EchoServer  echoserver(&loop,listen,n);
    echoserver.start();

    loop.loop();
}
