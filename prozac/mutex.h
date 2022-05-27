#ifndef __PROZAC_MUTEX_H__
#define __PROZAC_MUTEX_H__
#include <prozac/noncopyable.h>
#include <memory>
#include <semaphore.h>
#include <atomic>
namespace prozac
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
}

#endif