#include<muduo/net/EventLoop.h>
#include<muduo/net/EventLoopThread.h>
#include<muduo/base/Thread.h>
#include<muduo/base/Timestamp.h>

#include<unistd.h>
#include<stdio.h>


void printTid(){
    printf("test pid = %d ,tid = %d\n",::getpid(),muduo::CurrentThread::tid());
    printf("now = %s\n", muduo::Timestamp::now().toFormattedString().c_str());
}

void print(muduo::net::EventLoop * loop,std::string str){
    static int cnt = 0;
    printf("tid = %d , cnt = %d , str = %s\n",muduo::CurrentThread::tid(),cnt,str.c_str());
    if(++cnt==20) {
        loop->quit();
    }
}

void cancle(muduo::net::EventLoop * loop,muduo::net::TimerId timerid){
    loop->cancel(timerid);
    printf("cancelled at %s\n", muduo::Timestamp::now().toFormattedString().c_str());
}

int main(){
    printTid();
    sleep(1);
    {
        muduo::net::EventLoop loop;
        printf("main\n");
        auto id1=loop.runEvery(1.0,[&](){print(&loop,"every1");});
        auto id1_5=loop.runEvery(1.5,[&](){print(&loop,"every1.5");});
        loop.runEvery(2,[&](){print(&loop,"every2");});
        loop.runAfter(2.1,[&](){cancle(&loop,id1);});
        loop.runAfter(3.1,[&](){cancle(&loop,id1_5);});
        auto id4_5=loop.runEvery(4.5,[&](){print(&loop,"every4.5");});
        loop.runAfter(4.2,[&](){cancle(&loop,id4_5);});
        loop.loop();
        printf("main loop exits");
    }
    {
        muduo::net::EventLoopThread th;
        muduo::net::EventLoop * loop=th.startLoop();

        loop->runAfter(2,printTid);
        sleep(3);
        printf("thread loop exits");
    }
    int * p=static_cast<int*> (malloc( sizeof(int) ));
    (void)p;
}