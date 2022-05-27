#include <prozac/scheduler.h>
#include <iostream>
#include<stdio.h>
#include<unistd.h>
void test_scheduler(){
    std::cout << "pid: " << prozac::GetThreadId() << " say hello" <<std::endl;
}
int main(int argc,char** argv)
{
    prozac::Scheduler sch(8,"sch1");
    int k = 100000;
    while(k>0){
        prozac::Fiber::ptr f = prozac::Fiber::CreatFiber(test_scheduler);
        prozac::Scheduler::Task::ptr task(new prozac::Scheduler::Task(f));
        sch.submit(task);
        k--;
    }
    while(true)
    {

    }
    return 0;
}