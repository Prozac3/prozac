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
    int t = rand() % 1000;
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

void test()
{
    for (int i = 0; i < 4; i++)
    {
        int k = rand() % 3;
        switch (k)
        {
        case 0:
            test_scheduler();
        case 1:
            test_sleep();
        case 2:
            test_hold();
        default:
            test_scheduler();
        }
    }
}
int main(int argc, char **argv)
{
    srand(time(NULL));
    auto start = prozac::GetCurrentMS();
    prozac::Scheduler sch(12, "sch1");
    int k = 200000;
    while (k > 0)
    {
        prozac::Fiber::ptr f = prozac::Fiber::CreatFiber(test);
        prozac::Scheduler::Task::ptr task(new prozac::Scheduler::Task(std::move(f)));
        sch.submit(std::move(task));
        k--;
    }

    sch.stop();
    auto end = prozac::GetCurrentMS();
    std::cout << "running time: " << end - start << " ms" << std::endl;
    return 0;
}