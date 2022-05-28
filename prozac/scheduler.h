#ifndef __PROZAC_SCHEDULER_H__
#define __PROZAC_SCHEDULER_H__
#include <prozac/fiber.h>
#include <prozac/thread.h>
#include <memory>
#include <queue>
#include <vector>
#include <list>
#include <prozac/util.h>
#include <prozac/macro.h>
#include <random>
#include <atomic>
namespace prozac
{
    class Scheduler;
    class Scheduler
    {
    public:
        struct Task
        {
        public:
            typedef std::shared_ptr<Task> ptr;
            Fiber::ptr fiber;
            uint64_t lasttime;
            int thread;
            uint16_t priority;
            Task(Fiber::ptr f, int thr = -1, uint16_t pri = 5)
                : fiber(f), thread(thr), priority(pri)
            {
                lasttime = prozac::GetCurrentUS();
                if (PROZAC_UNLIKELY(pri > 9))
                {
                    priority = 9;
                }
            }

            bool operator>(const Task &t)
            {
                if (t.priority == this->priority)
                {
                    return this->lasttime < t.lasttime;
                }
                else
                {
                    uint16_t k = t.priority + this->priority + 2;
                    k = rand() % k;
                    return k < this->priority;
                }
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

        struct sleep_cmp
        {
            bool operator()(Task::ptr t1, Task::ptr t2)
            {
                return t1->fiber->getWaketime() > t2->fiber->getWaketime();
            }
        };

        class WokerThread;
        class AllocThread;
        class WokerThread : public Thread
        {
        public:
            friend class AllocThread;
            typedef std::shared_ptr<WokerThread> ptr;
            WokerThread(const std::string &name,std::atomic_bool& stop);

        private:
            static void *run(void *arg);

        private:
            std::atomic<int> m_count{0};
            std::string m_name;
            std::queue<Task::ptr> t_init;
            std::priority_queue<Task::ptr, std::vector<Task::ptr>, task_cmp> t_ready;
            std::priority_queue<Task::ptr, std::vector<Task::ptr>, sleep_cmp> t_sleep;
            std::list<Task::ptr> t_hold;
            Mutex m_mutex;
            std::atomic_bool& m_stop;
        };

        class AllocThread : public Thread
        {
        public:
            friend class WokerThread;
            friend class Scheduler;
            typedef std::shared_ptr<AllocThread> ptr;
            AllocThread(Mutex &mutex,
                        std::vector<WokerThread::ptr> &works,
                        std::queue<Task::ptr> &tasks,
                        std::atomic<uint64_t>& count,
                        std::atomic_bool& stop,
                        const std::string &name);

        private:
            static void *run(void *arg);

        private:
            Mutex &m_mutex;
            std::vector<WokerThread::ptr> &m_workers;
            std::queue<Task::ptr> &m_tasks;
            std::atomic<uint64_t>& m_count;
            std::atomic_bool& m_stop;
        };

    public:
        Scheduler(uint16_t count, const std::string &name);
        ~Scheduler();
        void stop();
        void submit(Scheduler::Task::ptr t);

    private:
        Mutex m_mutex;
        std::queue<Task::ptr> m_tasks;
        std::vector<WokerThread::ptr> m_workers;
        uint16_t m_thread_count;
        std::string m_name;
        AllocThread::ptr m_alloc;
        std::atomic_bool m_stop{false};
        std::atomic<uint64_t> m_count{0};
    };
}

#endif