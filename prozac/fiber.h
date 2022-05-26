#ifndef __PROZAC_FIBER_H__
#define __PROZAC_FIBER_H__
#include <stdint.h>
#include <ucontext.h>
#include <functional>
#include <memory>
namespace prozac
{
    class Fiber : std::enable_shared_from_this<Fiber>
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
            DESTROY
        };

    public:
        void yield();
        void sleep(uint64_t t);
        void resume();

    public:
        static Fiber::ptr CreatFiber(std::function<void()> cb, uint64_t size = 4096);
        static void SetThis(Fiber* fiber);
        static Fiber::ptr GetThis();
        static void MainFunc();
        static Fiber::ptr GetMainFiber();

    private:
        Fiber(std::function<void()> cb, uint64_t size = 4096);
        Fiber();

    private:
        uint64_t m_id = 0;
        ucontext_t m_ctx;
        uint64_t m_stacksize = 0;
        void *m_stack = nullptr;
        State m_state = INIT;
        std::function<void()> m_cb;
        int64_t waketime = 0;
    };
}

#endif