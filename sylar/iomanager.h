#ifndef __IOMANAGER_H__
#define __IOMANAGER_H__

#include <sylar/scheduler.h>
namespace sylar
{
    class IOManager : public Scheduler
    {
    public:
        typedef std::shared_ptr<IOManager> ptr;
        typedef RWMutex RWMutexType;
        enum Event
        {
            NONE = 0x0,
            READ = 0x1,
            WRITE = 0x2
        };

    private:
        struct FdContext
        {
            typedef Mutex MutexType;
            struct EventContext
            {
                Scheduler *scheduler = nullptr;
                Fiber::ptr fiber;
            };

            /**
             * @brief 获取事件上下文类
             * @param[in] event 事件类型
             * @return 返回对应事件的上线文
             */
            EventContext &getContext(Event event);

            /**
             * @brief 重置事件上下文
             * @param[in, out] ctx 待重置的上下文类
             */
            void resetContext(EventContext &ctx);

            /**
             * @brief 触发事件
             * @param[in] event 事件类型
             */
            void triggerEvent(Event event);

            EventContext read;
            EventContext write;
            int fd = 0;
            Event events = NONE;
            MutexType mutex;
        };

    public:
        IOManager(uint16_t count, const std::string &name);
        ~IOManager();

        /**
         * @brief 添加事件
         * @param[in] fd socket句柄
         * @param[in] event 事件类型
         * @param[in] cb 事件回调函数
         * @return 添加成功返回0,失败返回-1
         */
        int addEvent(int fd, Event event, std::function<void()> cb = nullptr);

        /**
         * @brief 删除事件
         * @param[in] fd socket句柄
         * @param[in] event 事件类型
         * @attention 不会触发事件
         */
        bool delEvent(int fd, Event event);

        /**
         * @brief 取消事件
         * @param[in] fd socket句柄
         * @param[in] event 事件类型
         * @attention 如果事件存在则触发事件
         */
        bool cancelEvent(int fd, Event event);

        /**
         * @brief 取消所有事件
         * @param[in] fd socket句柄
         */
        bool cancelAll(int fd);

        /**
         * @brief 返回当前的IOManager
         */
        static IOManager *GetThis();

    protected:
        void idle() override;

    private:
        void contextResize(size_t size);

    private:
        /// epoll 文件句柄
        int m_epfd = 0;
        /// pipe 文件句柄
        int m_tickleFds[2];
        /// 当前等待执行的事件数量
        std::atomic<size_t> m_pendingEventCount = {0};
        /// IOManager的Mutex
        RWMutexType m_mutex;
        /// socket事件上下文的容器
        std::vector<FdContext *> m_fdContexts;
    };
}

#endif