#ifndef __PROZAC_FIBER_H__
#define __PROZAC_FIBER_H__
#include <stdint.h>
#include <ucontext.h>
#include <functional>
#include <memory>
namespace prozac
{
    class Fiber : public std::enable_shared_from_this<Fiber>
    {
    public:
        typedef std::shared_ptr<Fiber> ptr;
        enum State
        {
            INIT,
            READY,
            EXEC,
            HOLD,
            SLEEP,
            DESTROY,
            EXCEPT
        };

    public:
        void start();
        void hold();
        void notify();
        void sleep(uint64_t t);
        void awake();
        void yield();
        void resume();
        void stop();
        void destroy();
        uint64_t getId() { return m_id; }
        State getState() const { return m_state; }
        uint64_t getWaketime() const { return waketime; }
        ~Fiber();

    public:
        static Fiber::ptr CreatFiber(std::function<void()> cb, uint64_t size = 4096);
        static void SetThis(Fiber *fiber);
        static Fiber::ptr GetThis();
        static void MainFunc();
        static Fiber::ptr GetMainFiber();
        static uint64_t GetFiberId();

    private:
        Fiber(std::function<void()> cb, uint64_t size = 4096);
        Fiber();

    private:
        uint64_t m_id = 0;
        ucontext_t m_ctx;
        std::function<void()> m_cb;
        uint64_t m_stacksize = 0;
        void *m_stack = nullptr;
        State m_state = INIT;
        uint64_t waketime = 0;
    };
}

#endif