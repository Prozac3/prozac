#include <prozac/scheduler.h>

namespace prozac
{
    void *Scheduler::WokerThread::run(void *arg)
    {
        WokerThread *thr = (WokerThread *)arg;
        SetThis((Thread *)thr);
        SetName(thr->m_name);
        thr->m_id = prozac::GetThreadId();
        pthread_setname_np(pthread_self(), thr->m_name.substr(0, 15).c_str());
        thr->m_semaphore.notify();
        Fiber::GetThis();
        while ((!thr->m_stop) || (thr->m_count > 0))
        {

            //尝试取出任务
            {
                if (thr->m_mutex.trylock())
                {
                    while (!thr->t_init.empty())
                    {
                        auto task = thr->t_init.front();
                        thr->t_init.pop();
                        task->fiber->start();
                        thr->t_ready.push(std::move(task));
                    }
                    thr->m_mutex.unlock();
                }
            }

            //休眠队列
            {
                auto t = prozac::GetCurrentMS();
                while (!thr->t_sleep.empty())
                {
                    auto task = thr->t_sleep.top();
                    if (task->fiber->getWaketime() < t)
                    {
                        task->fiber->awake();
                        thr->t_ready.push(std::move(task));
                        thr->t_sleep.pop();
                    }
                    else
                    {
                        break;
                    }
                }
            }

            //遍历挂起队列
            {
                size_t n = thr->t_hold.size();
                auto itr = thr->t_hold.begin();
                for (size_t i = 0; i < n; i++)
                {
                    auto task = *itr;
                    task->fiber->notify();
                    if (task->fiber->getState() == Fiber::READY)
                    {
                        thr->t_ready.push(std::move(task));
                        thr->t_hold.erase(itr++);
                    }
                    else
                    {
                        itr++;
                    }
                }
            }

            //拿出任务执行
            {
                if (!thr->t_ready.empty())
                {
                    auto task = thr->t_ready.top();
                    thr->t_ready.pop();
                    task->fiber->resume();
                    if (task->fiber->getState() == Fiber::HOLD)
                    {
                        thr->t_hold.push_back(std::move(task));
                    }
                    else if (task->fiber->getState() == Fiber::SLEEP)
                    {
                        thr->t_sleep.push(std::move(task));
                    }
                    else if (PROZAC_UNLIKELY(task->fiber->getState() == Fiber::READY))
                    {
                        thr->t_ready.push(std::move(task));
                    }
                    else
                    {
                        thr->m_count--;
                        task.reset();
                    }
                }
            }
        }
        return 0;
    }

    void *Scheduler::AllocThread::run(void *arg)
    {
        AllocThread *thr = (AllocThread *)arg;
        SetThis((Thread *)thr);
        SetName(thr->m_name);
        thr->m_id = prozac::GetThreadId();
        pthread_setname_np(pthread_self(), thr->m_name.substr(0, 15).c_str());
        thr->m_semaphore.notify();
        size_t k = 0;
        while (PROZAC_LIKELY((!thr->m_stop) || (thr->m_count > 0)))
        {
            if (PROZAC_UNLIKELY(thr->m_workers.size() == 0))
            {
                continue;
            }
            Mutex::Lock lock1(thr->m_mutex);
            if (k >= thr->m_workers.size())
            {
                k = 0;
            }
            WokerThread *woker = thr->m_workers[k].get();
            k++;
            Mutex::Lock lock2(woker->m_mutex);
            {
                if (woker->m_count < 100000)
                {
                    int l = 10;
                    while ((!thr->m_tasks.empty()) && l > 0)
                    {
                        auto task = thr->m_tasks.front();
                        thr->m_tasks.pop();
                        woker->t_init.push(std::move(task));
                        woker->m_count++;
                        thr->m_count--;
                        l--;
                    }
                }
            }
        }
        return 0;
    }

    Scheduler::WokerThread::WokerThread(const std::string &name, std::atomic_bool &stop)
        : m_name(name), m_stop(stop)
    {
        if (name.empty())
        {
            m_name = "UNKNOW";
        }
        t_hold.resize(0);
        t_init = std::queue<Task::ptr>();
        t_ready = std::priority_queue<Task::ptr, std::vector<Task::ptr>, task_cmp>();
        t_hold.resize(0);
        t_sleep = std::priority_queue<Scheduler::Task::ptr, std::vector<Task::ptr>, sleep_cmp>();
        int ret = pthread_create(&m_thread, nullptr, &WokerThread::run, this);
        if (ret)
        {
            throw std::logic_error("pthread_create error");
        }

        m_semaphore.wait();
    }

    Scheduler::AllocThread::AllocThread(Mutex &mutex,
                                        std::vector<WokerThread::ptr> &works,
                                        std::queue<Task::ptr> &tasks,
                                        std::atomic<uint64_t>& count,
                                        std::atomic_bool &stop,
                                        const std::string &name)
        : m_mutex(mutex),
          m_workers(works),
          m_tasks(tasks),
          m_count(count),
          m_stop(stop)  

    {
        m_name = name;
        if (m_name.empty())
        {
            m_name = "UNKNOW";
        }
        int ret = pthread_create(&m_thread, nullptr, &AllocThread::run, this);
        if (ret)
        {
            throw std::logic_error("pthread_create error");
        }

        m_semaphore.wait();
    }

    Scheduler::Scheduler(uint16_t count, const std::string &name)
        : m_thread_count(count),
          m_name(name)
    {
        for (int i = 0; i < count; i++)
        {
            WokerThread::ptr thr(new WokerThread(m_name + "_woker" + std::to_string(i), m_stop));
            m_workers.push_back(thr);
        }
        m_tasks = std::queue<Scheduler::Task::ptr>();
        m_alloc = AllocThread::ptr(new AllocThread(m_mutex, m_workers, m_tasks, m_count,m_stop, m_name + "_alloc"));
    }

    Scheduler::~Scheduler()
    {
        if(!m_stop){
            stop();
        }
    }

    void Scheduler::stop()
    {
        m_stop = true;
        for(auto woker:m_workers)
        {
            woker->join();
        }
        m_alloc->join();
    }

    void Scheduler::submit(Scheduler::Task::ptr t)
    {
        {
            if (PROZAC_LIKELY(!m_stop))
            {
                Mutex::Lock lock(m_mutex);
                if (m_tasks.size() < 10000)
                {
                    m_tasks.push(std::move(t));
                    m_count++;
                }
                else
                {
                    t->fiber->destroy();
                }
            }
        }
    }

}