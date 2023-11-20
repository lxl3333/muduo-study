#include<muduo/net/EventLoopThread.h>
#include<muduo/base/Timestamp.h>
#include<muduo/net/EventLoop.h>

#include<unistd.h>


using namespace muduo;
using namespace muduo::net;

void runinthread(){
    printf("main pid = %d, tid= %d ,now = %s\n",::getpid(),CurrentThread::tid(),
        Timestamp::now().toFormattedString().c_str());  
}

int main(){
    printf("main pid = %d, tid= %d ,now = %s\n",::getpid(),CurrentThread::tid(),
        Timestamp::now().toFormattedString().c_str());
    EventLoopThread et;
    EventLoop* loop= et.startLoop();
    
    loop->runAfter(1,runinthread);
    sleep(1);

    loop->runAfter(2,[&](){
        runinthread();
        //loop->quit();
    });
    sleep(3);
    printf("main pid = %d, tid= %d ,now = %s\n",::getpid(),CurrentThread::tid(),
        Timestamp::now().toFormattedString().c_str());
}