#ifndef __SYLAR_MUTEX_H__
#define __SYLAR_MUTEX_H__
#include <sylar/noncopyable.h>
#include <memory>
#include <semaphore.h>
#include <atomic>
namespace sylar
{
    class Semaphore : Noncopyable
    {
    public:
        Semaphore(uint32_t count = 0);
        ~Semaphore();
        void wait();
        void notify();

    private:
        sem_t m_semaphore;
    };

    template <class T>
    struct ScopedLock
    {
    public:
        ScopedLock(T &mutex)
            : m_mutex(mutex)
        {
            m_mutex.lock();
            m_locked = true;
        }

        ~ScopedLock()
        {
            unlock();
        }

        void lock()
        {
            if (!m_locked)
            {
                m_mutex.lock();
                m_locked = true;
            }
        }

        void unlock()
        {
            if (m_locked)
            {
                m_locked = false;
                m_mutex.unlock();
            }
        }

    private:
        T &m_mutex;
        bool m_locked;
    };

    template <class T>
    struct ReadScopedLock
    {
    public:
        /**
         * @brief 构造函数
         * @param[in] mutex 读写锁
         */
        ReadScopedLock(T &mutex)
            : m_mutex(mutex)
        {
            m_mutex.rdlock();
            m_locked = true;
        }

        /**
         * @brief 析构函数,自动释放锁
         */
        ~ReadScopedLock()
        {
            unlock();
        }

        /**
         * @brief 上读锁
         */
        void lock()
        {
            if (!m_locked)
            {
                m_mutex.rdlock();
                m_locked = true;
            }
        }

        /**
         * @brief 释放锁
         */
        void unlock()
        {
            if (m_locked)
            {
                m_mutex.unlock();
                m_locked = false;
            }
        }

    private:
        /// mutex
        T &m_mutex;
        /// 是否已上锁
        bool m_locked;
    };

    template <class T>
    struct WriteScopedLock
    {
    public:
        /**
         * @brief 构造函数
         * @param[in] mutex 读写锁
         */
        WriteScopedLock(T &mutex)
            : m_mutex(mutex)
        {
            m_mutex.wrlock();
            m_locked = true;
        }

        /**
         * @brief 析构函数
         */
        ~WriteScopedLock()
        {
            unlock();
        }

        /**
         * @brief 上写锁
         */
        void lock()
        {
            if (!m_locked)
            {
                m_mutex.wrlock();
                m_locked = true;
            }
        }

        /**
         * @brief 解锁
         */
        void unlock()
        {
            if (m_locked)
            {
                m_mutex.unlock();
                m_locked = false;
            }
        }

    private:
        /// mutex
        T &m_mutex;
        /// 是否已上锁
        bool m_locked;
    };

    /**
     * @brief 互斥量
     */
    class Mutex : Noncopyable
    {
    public:
        /// 局部锁
        typedef ScopedLock<Mutex> Lock;

        /**
         * @brief 构造函数
         */
        Mutex()
        {
            pthread_mutex_init(&m_mutex, nullptr);
        }

        /**
         * @brief 析构函数
         */
        ~Mutex()
        {
            pthread_mutex_destroy(&m_mutex);
        }

        /**
         * @brief 加锁
         */
        void lock()
        {
            pthread_mutex_lock(&m_mutex);
        }

        bool trylock()
        {
            return !pthread_mutex_trylock(&m_mutex);
        }

        /**
         * @brief 解锁
         */
        void unlock()
        {
            pthread_mutex_unlock(&m_mutex);
        }

    private:
        /// mutex
        pthread_mutex_t m_mutex;
    };

    /**
     * @brief 空锁(用于调试)
     */
    class NullMutex : Noncopyable
    {
    public:
        /// 局部锁
        typedef ScopedLock<NullMutex> Lock;

        /**
         * @brief 构造函数
         */
        NullMutex() {}

        /**
         * @brief 析构函数
         */
        ~NullMutex() {}

        /**
         * @brief 加锁
         */
        void lock() {}

        void trylock() {}

        /**
         * @brief 解锁
         */
        void unlock() {}
    };

    class RWMutex : Noncopyable
    {
    public:
        /// 局部读锁
        typedef ReadScopedLock<RWMutex> ReadLock;

        /// 局部写锁
        typedef WriteScopedLock<RWMutex> WriteLock;

        /**
         * @brief 构造函数
         */
        RWMutex()
        {
            pthread_rwlock_init(&m_lock, nullptr);
        }

        /**
         * @brief 析构函数
         */
        ~RWMutex()
        {
            pthread_rwlock_destroy(&m_lock);
        }

        /**
         * @brief 上读锁
         */
        void rdlock()
        {
            pthread_rwlock_rdlock(&m_lock);
        }

        bool tryrdlock()
        {
            return !pthread_rwlock_tryrdlock(&m_lock);
        }

        /**
         * @brief 上写锁
         */
        void wrlock()
        {
            pthread_rwlock_wrlock(&m_lock);
        }

        bool trywrlock()
        {
            return !pthread_rwlock_trywrlock(&m_lock);
        }
        /**
         * @brief 解锁
         */
        void unlock()
        {
            pthread_rwlock_unlock(&m_lock);
        }

    private:
        /// 读写锁
        pthread_rwlock_t m_lock;
    };

    class NullRWMutex : Noncopyable
    {
    public:
        /// 局部读锁
        typedef ReadScopedLock<NullMutex> ReadLock;
        /// 局部写锁
        typedef WriteScopedLock<NullMutex> WriteLock;

        /**
         * @brief 构造函数
         */
        NullRWMutex() {}
        /**
         * @brief 析构函数
         */
        ~NullRWMutex() {}

        /**
         * @brief 上读锁
         */
        void rdlock() {}

        bool tryrdlock() { return false; }
        /**
         * @brief 上写锁
         */
        void wrlock() {}

        bool trywrlock() { return false; }
        /**
         * @brief 解锁
         */
        void unlock() {}
    };

    /**
     * @brief 自旋锁
     */
    class Spinlock : Noncopyable
    {
    public:
        /// 局部锁
        typedef ScopedLock<Spinlock> Lock;

        /**
         * @brief 构造函数
         */
        Spinlock()
        {
            pthread_spin_init(&m_mutex, 0);
        }

        /**
         * @brief 析构函数
         */
        ~Spinlock()
        {
            pthread_spin_destroy(&m_mutex);
        }

        /**
         * @brief 上锁
         */
        void lock()
        {
            pthread_spin_lock(&m_mutex);
        }

        /**
         * @brief 解锁
         */
        void unlock()
        {
            pthread_spin_unlock(&m_mutex);
        }

    private:
        /// 自旋锁
        pthread_spinlock_t m_mutex;
    };

    /**
     * @brief 原子锁
     */
    class CASLock : Noncopyable
    {
    public:
        /// 局部锁
        typedef ScopedLock<CASLock> Lock;

        /**
         * @brief 构造函数
         */
        CASLock()
        {
            m_mutex.clear();
        }

        /**
         * @brief 析构函数
         */
        ~CASLock()
        {
        }

        /**
         * @brief 上锁
         */
        void lock()
        {
            while (std::atomic_flag_test_and_set_explicit(&m_mutex,
                                                          std::memory_order_acquire))
                ;
        }

        /**
         * @brief 解锁
         */
        void unlock()
        {
            std::atomic_flag_clear_explicit(&m_mutex, std::memory_order_release);
        }

    private:
        /// 原子状态
        volatile std::atomic_flag m_mutex;
    };

}

#endif