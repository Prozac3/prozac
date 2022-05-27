#include <prozac/thread.h>
#include <prozac/util.h>
namespace prozac
{
    static thread_local Thread *this_thread = nullptr;
    static thread_local std::string this_thread_name = "UNKNOW";

    Thread *Thread::GetThis()
    {
        return this_thread;
    }

    const std::string &Thread::GetName()
    {
        return this_thread_name;
    }

    void Thread::SetName(const std::string &name)
    {
        this_thread_name = name;
    }

    Thread::Thread(std::function<void()> cb, const std::string &name)
        : m_cb(cb), m_name(name)
    {
        if (name.empty())
        {
            m_name = "UNKNOW";
        }
        int ret = pthread_create(&m_thread, nullptr, &Thread::run, this);
        if (ret)
        {
            throw std::logic_error("pthread_create error");
        }

        m_semaphore.wait();
    }

    Thread::~Thread()
    {
        if (m_thread)
        {
            pthread_detach(m_thread);
        }
    }

    void Thread::join()
    {
        if (m_thread)
        {
            int ret = pthread_join(m_thread, nullptr);
            if (ret)
            {
                throw std::logic_error("pthread_join error");
            }
            m_thread = 0;
        }
    }
    void *Thread::run(void *arg)
    {
        Thread *thread = (Thread *)arg;
        this_thread = thread;
        this_thread_name = thread->m_name;
        thread->m_id = prozac::GetThreadId();
        pthread_setname_np(pthread_self(), thread->m_name.substr(0, 15).c_str());
        std::function<void()> cb;
        cb.swap(thread->m_cb);

        //子线程初始化完毕，唤醒主线程
        thread->m_semaphore.notify();
        cb();
        return 0;
    }

}