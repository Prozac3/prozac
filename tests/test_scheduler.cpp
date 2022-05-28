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
    int k = 5;
    while(true){
        prozac::Fiber::ptr f = prozac::Fiber::CreatFiber(test_scheduler);
        prozac::Scheduler::Task::ptr task(new prozac::Scheduler::Task(std::move(f)));
        sch.submit(std::move(task));
        k--;
    }
    return 0;
}