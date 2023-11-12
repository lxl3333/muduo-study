#include<muduo/base/Thread.h>
#include<muduo/base/CurrentThread.h>

#include <sys/types.h>
#include <unistd.h>


void fun1(){
    printf("tid=%d\n",muduo::CurrentThread::tid());
}

void fun2(int x){
    printf("tid=%d , x=%d\n",muduo::CurrentThread::tid(),x);
}

class Foo
{
public:
    Foo(double x):x_(x){
    };
    void member(){
        printf("tid=%d , x_=%f \n",muduo::CurrentThread::tid(),x_);
    };
    void memberwithtest(std::string str="lxl"){
        printf("tid=%d , x_=%f , testername=%s\n",muduo::CurrentThread::tid(),x_,str.c_str());
    }
private:
    double x_;
};

int main(){
    printf("pid=%d , tid=%d\n",::getpid(),muduo::CurrentThread::tid());

    muduo::Thread t1(fun1,"t1");
    t1.start();
    t1.join();

    muduo::Thread t2([](){fun2(2);},"t2");
    t2.start();
    t2.join();

    Foo foo(75.5);

    muduo::Thread t3([&](){foo.member();},"t3");
    t3.start();
    t3.join();

    muduo::Thread t4([&](){foo.memberwithtest("lxl0001");},"t4");
    t4.start();
    t4.join(); 

    printf("number of created threads %d\n",muduo::Thread::numCreated());
}