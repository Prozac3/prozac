#ifndef __PROZAC_FIBER_H__
#define __PROZAC_FIBER_H__
#include <stdint.h>
#include <ucontext.h>
#include <functional>
#include <memory>
namespace prozac
{
    //协程
    class Fiber : public std::enable_shared_from_this<Fiber>
    {
    public:
        typedef std::shared_ptr<Fiber> ptr;
        //协程状态
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
        /**
         * @brief 开始运行
         */
        void start();

        /**
         * @brief 挂起
         */
        void hold();

        /**
         * @brief 唤醒挂起协程
         */
        void notify();

        /**
         * @brief 休眠
         * @param t 休眠时间(ms)
         */
        void sleep(uint64_t t);

        /**
         * @brief 唤醒休眠协程
         */
        void awake();

        /**
         * @brief 切换至主协程
         */
        void yield();

        /**
         * @brief 恢复子线程
         */
        void resume();

        /**
         * @brief 终止协程(运行时主动中止)
         */
        void stop();

        /**
         * @brief 终止协程(主协程销毁)
         */
        void destroy();

        /**
         * @brief 获取协程ID
         */
        uint64_t getId() { return m_id; }

        /**
         * @brief 获取协程状态
         */
        State getState() const { return m_state; }

        /**
         * @brief 获取协程休眠终止时间(ms)
         */
        uint64_t getWaketime() const { return waketime; }
        ~Fiber();

    public:
        /**
         * @brief 创建协程
         * @param   cb 协程回调函数
         * @param size 协程栈大小
         */
        static Fiber::ptr CreatFiber(std::function<void()> cb, uint64_t size = 4096);

        /**
         * @brief 设置当前线程
         * @param fiber 当前协程指针
         */
        static void SetThis(Fiber *fiber);

        /**
         * @brief 获取当前协程
         */
        static Fiber::ptr GetThis();

        /**
         * @brief 协程执行函数
         */
        static void MainFunc();

        /**
         * @brief 获取主协程
         */
        static Fiber::ptr GetMainFiber();

        /**
         * @brief 获取当前协程ID
         */
        static uint64_t GetFiberId();

    private:
        /**
         * @brief Fiber
         * @param   cb 协程回调函数
         * @param size 协程栈大小
         */
        Fiber(std::function<void()> cb, uint64_t size = 4096);
        Fiber();

    private:
        uint64_t m_id = 0;          //协程ID
        ucontext_t m_ctx;           //协程上下文
        std::function<void()> m_cb; //协程回调函数
        uint64_t m_stacksize = 0;   //协程栈大小
        void *m_stack = nullptr;    //协程栈
        State m_state = INIT;       //协程状态
        uint64_t waketime = 0;      //协程休眠终止时间(仅SLEEP状态有效)
    };
}

#endif