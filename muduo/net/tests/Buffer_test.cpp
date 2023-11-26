#include<muduo/net/EventLoop.h>
#include<muduo/net/TcpServer.h>
#include<muduo/net/InetAddress.h>

#include<muduo/base/Logging.h>

#include<unistd.h>

using namespace muduo;
using namespace muduo::net;

class TestServer
{
public:
    TestServer(EventLoop* loop,const InetAddress& listen):loop_(loop),server_(loop,listen,"testserver")
    {
        server_.setConnectionCallback([&](const TcpConnectionPtr& conn){this->onConnection(conn);});
        server_.setMessageCallback([&](const TcpConnectionPtr& conn, Buffer* buf, Timestamp time){this->onMessage(conn,buf,time);});
        messageA.resize(100);
        messageB.resize(200);

        std::fill(messageA.begin(),messageA.end(),'A');
        std::fill(messageB.begin(),messageB.end(),'B');
    }

    void start(){
        server_.start();
    }

private:

    void onConnection(const TcpConnectionPtr& conn){
        if(conn->connected()){
            LOG_INFO<<"onConnection() :new connection ["<<conn->name().c_str()<<"] from "
                <<conn->peerAddress().toIpPort().c_str();
            conn->send(messageA);
            conn->send(messageB);
            conn->shutdown();
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
    TcpServer server_;

    std::string messageA;
    std::string messageB;
};

int main(){
    LOG_INFO<<"main() : pid = "<<::getpid();

    EventLoop loop;

    InetAddress listen(8888);

    TestServer ts(&loop,listen);

    ts.start();

    loop.loop();
}