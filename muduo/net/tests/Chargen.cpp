#include<muduo/net/EventLoop.h>
#include<muduo/net/TcpServer.h>
#include<muduo/net/InetAddress.h>
#include<muduo/base/Logging.h>

#include<unistd.h>

using namespace muduo;
using namespace muduo::net;


class Chargen{
public:
    Chargen(EventLoop* loop,const InetAddress& listen,bool print):
        transferred_(0),
        loop_(loop),
        server_(loop_,listen,"Chargen"),
        time_(Timestamp::now())
        
    {
        server_.setConnectionCallback([&](const TcpConnectionPtr& conn){
            this->onConnection(conn);
        });

        server_.setMessageCallback([&](const TcpConnectionPtr& conn,Buffer* buf,Timestamp time){
            this->onMessage(conn,buf,time);
        });

        server_.setWriteCompleteCallback([&](const TcpConnectionPtr & conn){
            this->onWriteComplete(conn);
        });

        if(print){
            loop_->runEvery(3,[&](){
                printftrans();
            });
        }

        string line;
        for (int i = 33; i < 127; ++i)
        {
            line.push_back(char(i));
        }
        line += line;

        for (size_t i = 0; i < 127-33; ++i)
        {
            message_ += line.substr(i, 72) + '\n';
        }


    }

    void start(){
        server_.start();
    }
private:
    void onConnection(const TcpConnectionPtr& conn){
        LOG_INFO<<"ChargenServer:"<<conn->peerAddress().toIpPort()<<"->"<<
            conn->localAddress().toIpPort()<<" is "<<(conn->connected()?"up":"down");
        if(conn->connected()){
            conn->setTcpNoDelay(true);
            conn->send(message_);
        }
    }

    void onMessage(const TcpConnectionPtr& conn,Buffer* buf,Timestamp time){
        string message=buf->retrieveAllAsString();
        LOG_INFO<<"ChargenServer: receive "<<message.size()<<"from "<<
            conn->localAddress().toIpPort();
    }


    void onWriteComplete(const TcpConnectionPtr & conn){
        transferred_+=message_.size();
        conn->send(message_);
    }
    void printftrans(){
        Timestamp end=Timestamp::now();
        double time=timeDifference(end,time_);
        printf("%4.3f MiB/s, %s\n", static_cast<double>(transferred_)/time/1024/1024,
            Timestamp::now().toFormattedString().c_str());
        transferred_=0;
        time_=end;
    }

    int64_t transferred_;
    std::string message_;
    EventLoop * loop_;
    TcpServer server_;
    Timestamp time_;
};



int main(){
    printf("main() pid = %d , now=%s" ,::getpid(),Timestamp::now().toFormattedString().c_str());
    EventLoop loop;
    InetAddress listen(8888);
    Chargen chargen(&loop,listen,true);

    chargen.start();

    loop.loop();
}

