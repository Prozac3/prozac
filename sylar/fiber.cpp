#include <sylar/fiber.h>
#include <sylar/macro.h>
#include <iostream>
#include <atomic>
namespace sylar
{
    static thread_local Fiber::ptr main_fiber = nullptr;
    static thread_local Fiber *this_fiber = nullptr;
    static thread_local std::atomic<uint64_t> fiberIds{0};
    static thread_local std::atomic<uint64_t> fiber_count{0};

    Fiber::ptr Fiber::CreatFiber(std::function<void()> cb, uint64_t size)
    {
        if (SYLAR_UNLIKELY(this_fiber == nullptr))
        {
            Fiber::ptr cur(new Fiber());
            SYLAR_ASSERT(this_fiber == cur.get());
            main_fiber = cur;
        }
        return Fiber::ptr(new Fiber(cb, size));
    }

    void Fiber::SetThis(Fiber *fiber)
    {
        this_fiber = fiber;
    }
    Fiber::ptr Fiber::GetThis()
    {
        if (this_fiber)
        {
            return this_fiber->shared_from_this();
        }

        Fiber::ptr cur(new Fiber);
        SYLAR_ASSERT(this_fiber == cur.get());
        main_fiber = cur;
        return this_fiber->shared_from_this();
    }
    void Fiber::MainFunc()
    {
        Fiber::ptr cur = GetThis();
        auto raw_ptr = cur.get();
        cur.reset();
        SYLAR_ASSERT(raw_ptr);
        try
        {
            raw_ptr->m_cb();
            raw_ptr->m_cb = nullptr;
        }
        catch (std::exception &ex)
        {
            raw_ptr->m_state = EXCEPT;
            std::cout << "Fiber Except: " << ex.what()
                      << " fiber_id=" << cur->getId()
                      << std::endl;
        }
        catch (...)
        {
            raw_ptr->m_state = EXCEPT;
            std::cout << "Fiber Except"
                      << " fiber_id=" << cur->getId()
                      << std::endl;
        }
        if (SYLAR_UNLIKELY(raw_ptr->m_state == EXCEPT))
        {
            SetThis(GetMainFiber().get());
            if (swapcontext(&raw_ptr->m_ctx, &main_fiber->m_ctx))
            {
                SYLAR_ASSERT2(false, "fail to stop");
            }
        }
        else
        {
            raw_ptr->stop();
        }
        SYLAR_ASSERT2(false, "never reach fiber_id=" + std::to_string(raw_ptr->getId()));
    }
    Fiber::ptr Fiber::GetMainFiber()
    {
        return main_fiber->shared_from_this();
    }

    uint64_t Fiber::GetFiberId()
    {
        if (this_fiber)
        {
            return this_fiber->getId();
        }
        return 0;
    }

    Fiber::Fiber(std::function<void()> cb, uint64_t size)
        : m_id(fiberIds), m_cb(cb), m_stacksize(size)
    {
        ++fiberIds;
        m_stack = malloc(m_stacksize);
        if (!m_stack)
        {
            return;
        }

        if (getcontext(&m_ctx))
        {
            SYLAR_ASSERT2(false, "getcontext");
        }
        m_ctx.uc_link = nullptr;
        m_ctx.uc_stack.ss_size = m_stacksize;
        m_ctx.uc_stack.ss_sp = m_stack;
        makecontext(&m_ctx, &Fiber::MainFunc, 0);
        ++fiber_count;
    }

    Fiber::Fiber()
    {
        m_state = EXEC;
        SetThis(this);
        if (getcontext(&m_ctx))
        {
            SYLAR_ASSERT2(false, "getcontext");
        }
        ++fiberIds;
        ++fiber_count;
    }

    Fiber::~Fiber()
    {
        --fiber_count;
        SYLAR_ASSERT(this_fiber == main_fiber.get());
        if (this != main_fiber.get())
        {
            SYLAR_ASSERT(m_state == DESTROY || m_state == EXCEPT);
            if (m_stack)
            {
                free(m_stack);
            }
        }
        else
        {
            if (m_stack)
            {
                free(m_stack);
            }
        }
      
    }

    void Fiber::start()
    {
        SYLAR_ASSERT2(GetThis()->m_id == GetMainFiber()->m_id,
                       "Fiber::start() must be called in main fiber.");
        SYLAR_ASSERT2(m_state == INIT,
                       "The Fiber that is not init cannot be started.");
        m_state = READY;
    }

    void Fiber::resume()
    {

        SYLAR_ASSERT2(GetThis()->m_id == GetMainFiber()->m_id,
                       "Fiber::resume() must be called in main fiber.");

        SetThis(this);

        SYLAR_ASSERT2(m_state == READY,
                       "The Fiber that is not ready cannot be resumed");
        m_state = EXEC;
        if (swapcontext(&main_fiber->m_ctx, &m_ctx))
        {
            SYLAR_ASSERT2(false, "fail to resume");
        }
    }

    void Fiber::yield()
    {
        SYLAR_ASSERT2(GetThis()->m_id != GetMainFiber()->m_id,
                       "Fiber::yield() must be called in child fiber.");
        SetThis(GetMainFiber().get());
        SYLAR_ASSERT2(m_state == EXEC,
                       "The Fiber that is not executing cannot be yielded");
        m_state = READY;
        if (swapcontext(&m_ctx, &main_fiber->m_ctx))
        {
            SYLAR_ASSERT2(false, "fail to yield");
        }
    }

    void Fiber::hold()
    {
        SYLAR_ASSERT2(GetThis()->m_id != GetMainFiber()->m_id,
                       "Fiber::hold() must be called in child fiber.");
        SetThis(GetMainFiber().get());
        SYLAR_ASSERT2(m_state == EXEC,
                       "The Fiber that is not executing cannot be yielded");
        m_state = HOLD;
        if (swapcontext(&m_ctx, &main_fiber->m_ctx))
        {
            SYLAR_ASSERT2(false, "fail to yield");
        }
    }

    void Fiber::notify()
    {

        SYLAR_ASSERT2(GetThis()->m_id == GetMainFiber()->m_id,
                       "Fiber::notify() must be called in main fiber.");

        SetThis(this);

        SYLAR_ASSERT2(m_state == HOLD,
                       "The Fiber that is not hold cannot be notified");
        m_state = EXEC;
        if (swapcontext(&main_fiber->m_ctx, &m_ctx))
        {
            SYLAR_ASSERT2(false, "fail to resume");
        }
    }

    void Fiber::stop()
    {
        SYLAR_ASSERT2(GetThis()->m_id != GetMainFiber()->m_id,
                       "Fiber::stop() must be called in child fiber.");
        SetThis(GetMainFiber().get());
        m_state = DESTROY;
        if (swapcontext(&m_ctx, &main_fiber->m_ctx))
        {
            SYLAR_ASSERT2(false, "fail to stop");
        }
    }

    void Fiber::sleep(uint64_t t)
    {
        SYLAR_ASSERT2(GetThis()->m_id != GetMainFiber()->m_id,
                       "Fiber::sleep() must be called in child fiber.");
        SetThis(GetMainFiber().get());
        SYLAR_ASSERT2(m_state == EXEC,
                       "The Fiber that is not executing cannot sleep");
        m_state = SLEEP;
        waketime = GetCurrentMS() + t;
        if (swapcontext(&m_ctx, &main_fiber->m_ctx))
        {
            SYLAR_ASSERT2(false, "fail to yield");
        }
    }

    void Fiber::awake()
    {
        SYLAR_ASSERT2(GetThis()->m_id == GetMainFiber()->m_id,
                       "Fiber::awake() must be called in main fiber.");
        SYLAR_ASSERT2(m_state == SLEEP,
                       "The Fiber that is not sleeping cannot be awoken");
        m_state = READY;
    }

    void Fiber::destroy()
    {
        SYLAR_ASSERT2(GetThis()->m_id == GetMainFiber()->m_id,
                       "Fiber::destroy() must be called in main fiber.");
        m_state = DESTROY;
    }
}