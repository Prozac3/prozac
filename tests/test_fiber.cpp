#include <sylar/fiber.h>
#include <iostream>

void test_fiber()
{
    auto cur = sylar::Fiber::GetThis();
    auto ptr = cur.get();
    cur.reset();
    ptr->sleep(1000);
    std::cout << "Hello World!" << std::endl;
}

void test_except()
{
    throw(8);
}

int main(int argc, char **argv)
{
    {
        sylar::Fiber::ptr fib1 = sylar::Fiber::CreatFiber(test_fiber);
        fib1->start();
        fib1->resume();
    }

    {
        sylar::Fiber::ptr fib1 = sylar::Fiber::CreatFiber(test_fiber);
        fib1->start();
        fib1->resume();
    }
    

    std::cout << "return" << std::endl;
    return 0;
}