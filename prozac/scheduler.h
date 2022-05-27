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
namespace prozac
{
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
        class WokerThread;
        class AllocThread;
        class WokerThread : public Thread
        {
        public:
            friend class AllocThread;
            typedef std::shared_ptr<WokerThread> ptr;
            WokerThread(const std::string &name);

        private:
            static void *run(void *arg);

        private:
            int m_count;
            std::string m_name;
            std::queue<Task::ptr> t_init;
            std::priority_queue<Task::ptr> t_ready;
            std::priority_queue<Task::ptr> t_sleep;
            std::list<Task::ptr> t_hold;
            std::queue<Task::ptr> t_pool;
            Mutex m_mutex;
        };

        class AllocThread : public Thread
        {
        public:
            friend class WokerThread;
            AllocThread(Mutex &mutex,
                        std::vector<WokerThread::ptr> &works,
                        std::queue<Task::ptr>& tasks,
                        const std::string &name);

        private:
            static void *run(void *arg);

        private:
            Mutex &m_mutex;
            std::vector<WokerThread::ptr> &m_workers;
            std::queue<Task::ptr> &m_tasks;
        };
    public:
        Scheduler(uint16_t count,const std::string&name);
        ~Scheduler();

        void submit(Scheduler::Task::ptr t);

    private:
        Mutex m_mutex;
        std::queue<Task::ptr> m_tasks;
        std::vector<WokerThread::ptr> m_workers;
        uint16_t m_thread_count;
        std::string m_name;
        AllocThread::ptr m_alloc;
    };
}

#endif