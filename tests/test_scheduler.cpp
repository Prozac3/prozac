#include <sylar/scheduler.h>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
sylar::Mutex mut;
void test_scheduler()
{
    mut.lock();
    std::cout << "pid: " << sylar::GetThreadId() << " say hello" << std::endl;
    mut.unlock();
}

void test_sleep()
{
    mut.lock();
    int t = rand() % 1000;
    std::cout << "pid: " << sylar::GetThreadId()
              << " fiber_id: " << sylar::GetFiberId()
              << "   sleep for " << t << "ms" << std::endl;
    mut.unlock();
    sylar::Fiber::GetThis()->sleep(t);
    mut.lock();
    std::cout << "pid: " << sylar::GetThreadId()
              << " fiber_id: " << sylar::GetFiberId()
              << " wake" << std::endl;
    mut.unlock();
}

void test_hold()
{
    mut.lock();
    std::cout << "pid: " << sylar::GetThreadId()
              << " fiber_id: " << sylar::GetFiberId()
              << "   hold " << std::endl;
    mut.unlock();
    sylar::Fiber::GetThis()->hold();

    while (rand() % 1000 < 950)
    {
        sylar::Fiber::GetThis()->hold();
    }

    mut.lock();
    std::cout << "pid: " << sylar::GetThreadId()
              << " fiber_id: " << sylar::GetFiberId()
              << "   ready" << std::endl;
    mut.unlock();

    sylar::Fiber::GetThis()->yield();
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
    auto start = sylar::GetCurrentMS();
    sylar::Scheduler sch(12, "sch1");
    int k = 200000;
    while (k > 0)
    {
        sylar::Fiber::ptr f = sylar::Fiber::CreatFiber(test);
        sylar::Scheduler::Task::ptr task(new sylar::Scheduler::Task(std::move(f)));
        sch.submit(std::move(task));
        k--;
    }

    sch.stop();
    auto end = sylar::GetCurrentMS();
    std::cout << "running time: " << end - start << " ms" << std::endl;
    return 0;
}