#include <prozac/fiber.h>
#include <iostream>

void test_fiber()
{
    auto cur = prozac::Fiber::GetThis();
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
        prozac::Fiber::ptr fib1 = prozac::Fiber::CreatFiber(test_fiber);
        fib1->start();
        fib1->resume();
    }

    {
        prozac::Fiber::ptr fib1 = prozac::Fiber::CreatFiber(test_fiber);
        fib1->start();
        fib1->resume();
    }
    

    std::cout << "return" << std::endl;
    return 0;
}