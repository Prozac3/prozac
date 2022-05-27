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
        while (true)
        {

            //尝试取出任务
            {
                if (thr->m_mutex.trylock())
                {
                    while (!thr->t_init.empty())
                    {
                        auto p = thr->t_init.front();
                        thr->t_init.pop();
                        p->fiber->setState(Fiber::READY);
                        thr->t_ready.push(p);
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
                        thr->t_hold.push_back(task);
                    }
                    else if (task->fiber->getState() == Fiber::SLEEP)
                    {
                        thr->t_sleep.push(task);
                    }
                    else
                    {
                        thr->t_pool.push(task);
                    }
                }
            }

            //休眠队列
            {
                auto t = prozac::GetCurrentMS();
                while (!thr->t_sleep.empty())
                {
                    auto task = thr->t_sleep.top();
                    if (task->fiber->getWaketime() > t)
                    {
                        task->fiber->setState(Fiber::READY);
                        thr->t_ready.push(task);
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
                    task->fiber->resume();
                    if (task->fiber->getState() == Fiber::READY)
                    {
                        thr->t_hold.erase(itr);
                        thr->t_ready.push(task);
                    }
                    else
                    {
                        itr++;
                    }
                }
            }
        }
        return 0;
    }

    Scheduler::WokerThread::WokerThread(const std::string& name)
    :m_name(name)
    {
        if(name.empty())
        {
            m_name = "UNKNOW";
        }

        int ret = pthread_create(&m_thread, nullptr, &WokerThread::run, this);
        if (ret)
        {
            throw std::logic_error("pthread_create error");
        }

        m_semaphore.wait();
    }
}