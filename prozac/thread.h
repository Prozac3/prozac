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
        /**
         * @brief Construct a new Thread object
         *
         * @param cb    回调函数
         * @param name  线程名称
         */
        Thread(std::function<void()> cb, const std::string &name);
        Thread();
        virtual ~Thread();

        /**
         * @brief 获取线程ID
         *
         * @return pid_t 线程ID
         */
        pid_t getId() const { return m_id; };

        /**
         * @brief 获取线程名称
         *
         * @return const std::string& 线程名称
         */
        const std::string &getName() const { return m_name; };

        /**
         * @brief join
         *
         */
        void join();

        /**
         * @brief 获取线程指针
         *
         * @return Thread*  线程指针
         */
        static Thread *GetThis();

        /**
         * @brief 设置线程指针
         *
         * @param thread    线程指针
         */
        static void SetThis(Thread *thread);

        /**
         * @brief 获取线程名称
         *
         * @return const std::string&  线程名称
         */
        static const std::string &GetName();

        /**
         * @brief 设置线程名称
         *
         * @param name 名称
         */
        static void SetName(const std::string &name);

    private:
        Thread(const Thread &) = delete;
        Thread(const Thread &&) = delete;
        Thread operator=(const Thread &) = delete;
        static void *run(void *arg);

    protected:
        pid_t m_id;                 //线程PID
        pthread_t m_thread;         //线程
        std::function<void()> m_cb; //回调函数
        std::string m_name;         //线程名称
        Semaphore m_semaphore;      //原子锁
    };
}

#endif