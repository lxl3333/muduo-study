#include<muduo/net/Acceptor.h>
#include<muduo/net/EventLoop.h>
#include<muduo/net/InetAddress.h>
#include<muduo/net/SocketsOps.h>
#include<muduo/base/Timestamp.h>

#include<unistd.h>

using namespace muduo;
using namespace muduo::net;

void newConnnection(int sockfd,const InetAddress& peerAddr){
    printf("newConnection():from InetAddress:%s now = %s\n",peerAddr.toIpPort().c_str(),
        Timestamp::now().toFormattedString().c_str());
    ssize_t error=::write(sockfd,"How are you?\n",13);
    (void)error;
    sockets::close(sockfd);
}


int main(){
    printf("main():pid = %d ,now = %s\n" ,::getpid(),Timestamp::now().toFormattedString().c_str());

    InetAddress listenAddr(8888);
    EventLoop loop;
    Acceptor acceptor(&loop,listenAddr,false);
    acceptor.setNewConnectionCallback(newConnnection);
    acceptor.listen();
    loop.loop();

}