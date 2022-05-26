#include <prozac/fiber.h>
#include <prozac/macro.h>
namespace prozac
{
    static thread_local Fiber::ptr main_fiber;
    static thread_local Fiber *this_fiber;
    static thread_local uint64_t fiberIds;

    Fiber::ptr Fiber::CreatFiber(std::function<void()> cb, uint64_t size)
    {
        if (PROZAC_UNLIKELY(main_fiber == nullptr))
        {
            Fiber::ptr cur(new Fiber());
            PROZAC_ASSERT(this_fiber = cur.get());
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

        Fiber::ptr cur(new Fiber());
        PROZAC_ASSERT(this_fiber = cur.get());
        main_fiber = cur;
        return this_fiber->shared_from_this();
    }
    void Fiber::MainFunc()
    {
    }
    Fiber::ptr Fiber::GetMainFiber()
    {
        return main_fiber;
    }

    Fiber::Fiber(std::function<void()> cb, uint64_t size)
        : m_cb(cb), m_stacksize(size)
    {
        ++fiberIds;
        m_stack = malloc(m_stacksize);
        if (!m_stack)
        {
            return;
        }

        m_ctx.uc_link = nullptr;
        m_ctx.uc_stack.ss_size = m_stacksize;
        m_ctx.uc_stack.ss_sp = m_stack;
        makecontext(&m_ctx, &Fiber::MainFunc, 0);
        m_id = fiberIds;
    }

    Fiber::Fiber()
    {
        m_state = EXEC;
        SetThis(this);
        if (getcontext(&m_ctx))
        {
            PROZAC_ASSERT2(false, "getcontext");
        }
        ++fiberIds;
    }

    void Fiber::resume()
    {
        PROZAC_ASSERT2(GetThis()->m_id == GetMainFiber()->m_id,
                       "Fiber::resume() must be called in main fiber.");
        SetThis(this);
        PROZAC_ASSERT2(m_state == READY,
                       "The Fiber that is not ready cannot be resumed");
        m_state = EXEC;
        if (swapcontext(&Fiber::GetMainFiber()->m_ctx, &m_ctx))
        {
            PROZAC_ASSERT2(false, "fail to resume");
        }
    }

    void Fiber::yield()
    {
        PROZAC_ASSERT2(GetThis()->m_id != GetMainFiber()->m_id,
                       "Fiber::yield() must be called in child fiber.");
        SetThis(this);
        PROZAC_ASSERT2(m_state == EXEC,
                       "The Fiber that is not executing cannot be yielded");
        m_state = HOLD;
        if (swapcontext(&Fiber::GetMainFiber()->m_ctx, &m_ctx))
        {
            PROZAC_ASSERT2(false, "fail to yield");
        }
    }

    void Fiber::sleep(uint64_t t)
    {
        PROZAC_ASSERT2(GetThis()->m_id != GetMainFiber()->m_id,
                       "Fiber::sleep() must be called in child fiber.");
        SetThis(this);
        PROZAC_ASSERT2(m_state == EXEC,
                       "The Fiber that is not executing cannot sleep");
        m_state = SLEEP;
        waketime = GetCurrentUS() + t;
        if (swapcontext(&Fiber::GetMainFiber()->m_ctx, &m_ctx))
        {
            PROZAC_ASSERT2(false, "fail to yield");
        }
    }
}