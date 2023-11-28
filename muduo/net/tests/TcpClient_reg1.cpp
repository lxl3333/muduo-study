#include<muduo/net/TcpClient.h>
#include<muduo/net/EventLoop.h>
#include<muduo/net/InetAddress.h>

#include<muduo/base/Logging.h>

using namespace muduo;
using namespace muduo::net;

void timeout(TcpClient* client){
    LOG_INFO<<"timerout";
    client->stop();
}


int main(int argc,char *argv[]){
    EventLoop loop;
    InetAddress serverAddr("127.0.0.1",2);
    TcpClient client(&loop,serverAddr,"tcpCLient");

    loop.runAfter(0.0,[&](){
        timeout(&client);
    });

    loop.runAfter(1.0,[&](){
        loop.quit();
    });
    client.connect();

    CurrentThread::sleepUsec(100*1000);

    loop.loop();

}