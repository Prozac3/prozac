#include <prozac/scheduler.h>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
prozac::Mutex mut;
void test_scheduler()
{
    mut.lock();
    std::cout << "pid: " << prozac::GetThreadId() << " say hello" << std::endl;
    mut.unlock();
}

void test_sleep()
{
    mut.lock();
    int t = rand() % 10000;
    std::cout << "pid: " << prozac::GetThreadId()
              << " fiber_id: " << prozac::GetFiberId()
              << "   sleep for " << t << "ms" << std::endl;
    mut.unlock();
    prozac::Fiber::GetThis()->sleep(t);
    mut.lock();
    std::cout << "pid: " << prozac::GetThreadId()
              << " fiber_id: " << prozac::GetFiberId()
              << " wake" << std::endl;
    mut.unlock();
}

void test_hold()
{
    mut.lock();
    std::cout << "pid: " << prozac::GetThreadId()
              << " fiber_id: " << prozac::GetFiberId()
              << "   hold " << std::endl;
    mut.unlock();
    prozac::Fiber::GetThis()->hold();

    while (rand() % 1000 < 950)
    {
        prozac::Fiber::GetThis()->hold();
    }


    mut.lock();
    std::cout << "pid: " << prozac::GetThreadId()
              << " fiber_id: " << prozac::GetFiberId()
              << "   ready" << std::endl;
    mut.unlock();

    prozac::Fiber::GetThis()->yield();
}
int main(int argc, char **argv)
{
    srand(time(NULL));
    prozac::Scheduler sch(8, "sch1");
    int k = 1000000;
    while (true)
    {
        prozac::Fiber::ptr f = prozac::Fiber::CreatFiber(test_hold);
        prozac::Scheduler::Task::ptr task(new prozac::Scheduler::Task(std::move(f)));
        sch.submit(std::move(task));
        k--;
    }
    while (true)
    {
        /* code */
    }

    return 0;
}