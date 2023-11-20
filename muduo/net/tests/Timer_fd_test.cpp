#include<muduo/net/Channel.h>
#include<muduo/net/EventLoop.h>
#include<muduo/base/Timestamp.h>

#include<unistd.h>
#include<sys/types.h>
#include<sys/timerfd.h>



void timerfd_callback(muduo::net::EventLoop * loop,int timerfd){
    printf("timerfd_callback pid = %d , tid =  %d ,time = %s\n",::getpid(),muduo::CurrentThread::tid()
        ,muduo::Timestamp::now().toFormattedString().c_str());
    static uint64_t i=0;
    uint64_t exp=0;
    ssize_t s;
    //sleep(5);
    s =read(timerfd,&exp,sizeof(exp));
    (void)s;
    i+=exp;
    if(i>=10) loop->quit();
}

int main(){
    muduo::net::EventLoop loop;
    printf("timerfd_callback pid = %d , tid =  %d ,time = %s\n",::getpid(),muduo::CurrentThread::tid()
        ,muduo::Timestamp::now().toFormattedString().c_str());
    int timerfd=::timerfd_create(CLOCK_MONOTONIC,TFD_NONBLOCK|TFD_CLOEXEC);

    muduo::net::Channel channel(&loop,timerfd);
    std::function<void (muduo::Timestamp)> lambda=[&](muduo::Timestamp){timerfd_callback(&loop,timerfd);};
    channel.setReadCallback(lambda);
    channel.enableReading();

    struct itimerspec howlong;
    bzero(&howlong,sizeof(howlong));
    howlong.it_value.tv_sec = 5;
    howlong.it_value.tv_nsec=0;
    howlong.it_interval.tv_sec = 1;
    howlong.it_interval.tv_nsec = 0;
    ::timerfd_settime(timerfd, 0, &howlong, NULL);

    loop.loop();

    ::close(timerfd);

}