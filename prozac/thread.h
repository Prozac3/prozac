#ifndef __PROZAC_THREAD_H__
#define __PROZAC_THREAD_H__

#include <pthread.h>
#include <prozac/mutex.h>
#include <memory>
#include <functional>

namespace prozac
{
    class Thread
    {
    public:
        typedef std::shared_ptr<Thread> ptr;
        Thread(std::function<void()> cb, const std::string &name);
        Thread();
        ~Thread();
        pid_t getId() const { return m_id; };
        const std::string &getName() const { return m_name; };
        void join();
        static Thread *GetThis();
        static void SetThis(Thread *thread);
        static const std::string &GetName();
        static void SetName(const std::string &name);

    private:
        Thread(const Thread &) = delete;
        Thread(const Thread &&) = delete;
        Thread operator=(const Thread &) = delete;
        static void *run(void *arg);

    protected:
        pid_t m_id;
        pthread_t m_thread;
        std::function<void()> m_cb;
        std::string m_name;
        Semaphore m_semaphore;
    };
}

#endif