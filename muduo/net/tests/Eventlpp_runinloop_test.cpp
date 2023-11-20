#include<muduo/net/EventLoop.h>
#include<muduo/base/Timestamp.h>

#include<unistd.h>
#include<memory>


using namespace muduo;
using namespace muduo::net;

void run5(EventLoop* loop){
    printf("run5 start: pid= %d , time = %s\n",::getpid(),Timestamp::now().toFormattedString().c_str());
    loop->quit();
}

void run4(EventLoop* loop){
    printf("run4 start: pid= %d , time = %s\n",::getpid(),Timestamp::now().toFormattedString().c_str());
    loop->runAfter(3,[=](){
        run5(loop);
    });
}

void run3(EventLoop* loop){
    printf("run3 start: pid= %d , time = %s\n",::getpid(),Timestamp::now().toFormattedString().c_str());
    loop->queueInLoop([=](){
        run4(loop);
    });
}


void run2(EventLoop* loop){
    printf("run2 start: pid= %d , time = %s\n",::getpid(),Timestamp::now().toFormattedString().c_str());
    loop->runInLoop([=](){
        run3(loop);
    });
}

void run1(EventLoop* loop){
    printf("run1 start: pid= %d , time = %s\n",::getpid(),Timestamp::now().toFormattedString().c_str());
    loop->runInLoop([=](){
        run2(loop);
    });
}

int main(){
    printf("main start: pid= %d , time = %s\n",::getpid(),Timestamp::now().toFormattedString().c_str());
    EventLoop loop;
    loop.runAfter(2,[&](){
        run1(&loop);
    });
    loop.loop();
    printf("main quit: pid= %d , time = %s\n",::getpid(),Timestamp::now().toFormattedString().c_str());
    std::shared_ptr<int*> p;
}