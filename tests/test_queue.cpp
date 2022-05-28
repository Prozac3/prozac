#include <queue>
#include <stdlib.h>
#include <stdio.h>
#include <list>
#include <iostream>
#include <memory>

struct Task
{
    typedef std::shared_ptr<Task> ptr;
    std::string name;
    uint64_t lasttime;
    uint16_t priority;
    Task(const std::string &n, uint64_t last, uint16_t prio)
        : name(n), lasttime(last), priority(prio)
    {
    }
    std::string toString()
    {
        return name + "  " +
               std::to_string(priority) + "  " +
               std::to_string(lasttime);
    }
};

struct task_cmp
{
    bool operator()(Task::ptr t1, Task::ptr t2)
    {
        if (t1->priority == t2->priority)
        {
            return t1->lasttime > t2->lasttime;
        }
        else
        {
            uint16_t k = t1->priority + t2->priority + 2;
                    k = rand() % k;
            return t1->priority < k;
        }
    }
};

int main(int argc, char **argv)
{
    srand(time(NULL));
    std::priority_queue<Task::ptr, std::vector<Task::ptr>, task_cmp> que;
    que.push(Task::ptr(new Task("t1", 4, 5)));
    que.push(Task::ptr(new Task("t2", 5, 5)));
    que.push(Task::ptr(new Task("t3", 5, 5)));
    que.push(Task::ptr(new Task("t4", 4, 1000)));
    que.push(Task::ptr(new Task("t5", 5, 1000)));
    que.push(Task::ptr(new Task("t6", 4, 1000)));
    while (!que.empty())
    {
        std::cout << que.top()->toString()<<std::endl;
        que.pop();
    }
    return 0;
}