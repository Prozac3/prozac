#include <prozac/fiber.h>
#include <iostream>

void test_fiber()
{
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

    while (true)
    {
        prozac::Fiber::ptr fib2 = prozac::Fiber::CreatFiber(test_except);
        fib2->start();
        fib2->resume();
        std::cout << "line35: " << prozac::Fiber::GetThis()->getId() << std::endl;
    }

    std::cout << "return" << std::endl;
    return 0;
}