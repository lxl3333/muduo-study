#include<muduo/net/EventLoop.h>
#include<muduo/base/Timestamp.h>
#include<muduo/base/Thread.h>

#include<sys/types.h>
#include<unistd.h>

using namespace muduo;
using namespace muduo::net;


void ThreadFunc(){
    printf("ThreadFunc pid = %d , tid = %d\n",::getpid(),muduo::CurrentThread::tid());
    EventLoop loop;
    loop.loop();
}

void ThreadFunc3(){
    EventLoop loop;
}

void ThreadFunc2(){
    printf("ThreadFunc pid = %d , tid = %d , time = %s\n",::getpid(),muduo::CurrentThread::tid(),
        muduo::Timestamp::now().toFormattedString().c_str());

    EventLoop loop;
    loop.runEvery(0.99,[]{
        printf("ThreadFunc pid =%d , tid = %d , time= %s\n",::getpid()
        ,muduo::CurrentThread::tid(),muduo::Timestamp::now().toFormattedString().c_str());
    });
    //loop.runAfter(15,ThreadFunc3);
    //sleep(10);
    loop.loop();
}


void ThreadFunc4(EventLoop* loop){
    loop->loop();
}



int main(){
    EventLoop loop;
    

    Thread td(ThreadFunc,"thread1");
    td.start();


    Thread td2(ThreadFunc2,"thread2");
    td2.start();

    Thread td3([&](){
        sleep(10);
        ThreadFunc4(&loop);   
    },"thread3");
    td3.start();
    
    td.join();
    td2.join();
    td3.join();
    return 0;
}