#ifndef __SYLAR_SCHEDULER_H__
#define __SYLAR_SCHEDULER_H__
#include <sylar/fiber.h>
#include <sylar/thread.h>
#include <memory>
#include <queue>
#include <vector>
#include <list>
#include <sylar/util.h>
#include <sylar/macro.h>
#include <random>
#include <atomic>
namespace sylar
{
    //多线程协程调度器
    class Scheduler
    {
    public:
        struct Task
        {
        public:
            typedef std::shared_ptr<Task> ptr;
            Fiber::ptr fiber;  //任务协程
            uint64_t lasttime; //任务最后处理时间
            int thread;        //指定任务运行线程
            uint16_t priority; //任务优先级
            Task(Fiber::ptr f, int thr = -1, uint16_t pri = 5)
                : fiber(f), thread(thr), priority(pri)
            {
                lasttime = sylar::GetCurrentUS();
                if (SYLAR_UNLIKELY(pri > 9))
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

        //任务执行优先级
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

        //睡眠唤醒优先级
        struct sleep_cmp
        {
            bool operator()(Task::ptr t1, Task::ptr t2)
            {
                return t1->fiber->getWaketime() > t2->fiber->getWaketime();
            }
        };

        class WokerThread;
        class AllocThread;

        //工作线程
        class WokerThread : public Thread
        {
        public:
            friend class AllocThread;
            typedef std::shared_ptr<WokerThread> ptr;

            /**
             * @brief WokerThread
             * @param name 线程名称
             * @param stop 线程开关
             */
            WokerThread(const std::string &name, std::atomic_bool &stop);

        private:
            /**
             * @brief 线程启动函数
             * @param arg 线程指针
             */
            static void *run(void *arg);

        private:
            std::atomic<int> m_count{0};  //当前线程任务总量
            std::string m_name;           //线程名称
            std::queue<Task::ptr> t_init; // INIT状态任务

            std::priority_queue<Task::ptr,
                                std::vector<Task::ptr>,
                                task_cmp>
                t_ready; // REDEAY状态任务

            std::priority_queue<Task::ptr,
                                std::vector<Task::ptr>,
                                sleep_cmp>
                t_sleep; // SLEEP状态任务

            std::list<Task::ptr> t_hold; // HOLD状态任务
            Mutex m_mutex;               //线程锁
            std::atomic_bool &m_stop;    //线程开关
        };

        //任务分配线程
        class AllocThread : public Thread
        {
        public:
            friend class WokerThread;
            friend class Scheduler;
            typedef std::shared_ptr<AllocThread> ptr;
            /**
             * @brief AllocThread
             * @param tasks 任务队列
             * @param count 任务数量
             * @param stop 线程开关
             * @param name 线程名称
             */
            AllocThread(Mutex &mutex,
                        std::vector<WokerThread::ptr> &works,
                        std::queue<Task::ptr> &tasks,
                        std::atomic<uint64_t> &count,
                        std::atomic_bool &stop,
                        const std::string &name);

        private:
            /**
             * @brief 线程启动函数
             * @param arg 线程指针
             */
            static void *run(void *arg);

        private:
            Mutex &m_mutex;                           //任务队列锁
            std::vector<WokerThread::ptr> &m_workers; //工作线程
            std::queue<Task::ptr> &m_tasks;           //任务队列
            std::atomic<uint64_t> &m_count;           //任务总数
            std::atomic_bool &m_stop;                 //线程开关
        };

    public:
        /**
         * @brief Scheduler
         * @param count 工作线程数量
         * @param name 调度器名称
         */
        Scheduler(uint16_t count, const std::string &name);
        ~Scheduler();
        /**
         * @brief 停止调度
         */
        void stop();

        /**
         * @brief 提交任务
         * @param task 任务
         */
        void submit(Scheduler::Task::ptr task);

    private:
        Mutex m_mutex;                           //任务队列锁
        std::queue<Task::ptr> m_tasks;           //任务队列
        std::vector<WokerThread::ptr> m_workers; //工作线程
        uint16_t m_thread_count;                 //工作线程数量
        std::string m_name;                      //调度器名称
        AllocThread::ptr m_alloc;                //任务分配线程
        std::atomic_bool m_stop{false};          //调度器开关
        std::atomic<uint64_t> m_count{0};        //任务数量
    };
}

#endif