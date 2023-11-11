#include<muduo/base/ThreadPool.h>
#include<muduo/base/CurrentThread.h>
#include<muduo/base/Mutex.h>

#include <string>
#include <unistd.h>
void print(std::string str){
    printf("ThreadId=%d,%s\n",muduo::CurrentThread::tid(),str.c_str());
}




int main(){
    muduo::ThreadPool tp("test");
    tp.start(100);

    auto lanbda=[&]{
        print("你好");
    };
    tp.run(lanbda);
    tp.run(lanbda);
    muduo::MutexLock lock;
    int j=0;
    for(int i=0;i<100000;i++){
        tp.run([i,&j,&lock]{
            std::string str="task ";
            str+=std::to_string(i);
            printf("tid=%d, str=%s\n", muduo::CurrentThread::tid(), str.c_str());
            {
                muduo::MutexLockGuard lockguard(lock);
                j++;
            }
        });
    }
    muduo::CountDownLatch latch(1);
    tp.run([&]{latch.countDown();});
    latch.wait();

    tp.stop();





    printf("%d\n",j);
}