#ifndef __SYLAR_LOG_H__
#define __SYLAR_LOG_H__
#include <string.h>
#include <memory>
#include <stdint.h>
#include <sstream>
#include <vector>
#include <list>
#include <sylar/mutex.h>
#include <fstream>
#include <map>
#include <sylar/util.h>

#define SYLAR_LOG_LEVEL(logger, level)                                   \
    if (true)                                                             \
    sylar::LogEventWrap(sylar::LogEvent::ptr(new sylar::LogEvent(      \
                             logger, level, __FILE__, __LINE__, 0,        \
                             sylar::GetThreadId(), sylar::GetFiberId(), \
                             time(0), sylar::Thread::GetName())))        \
        .getSS()

#define SYLAR_LOG_DEBUG(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::DEBUG)

#define SYLAR_LOG_INFO(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::INFO)

#define SYLAR_LOG_WARN(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::WARN)

#define SYLAR_LOG_ERROR(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::ERROR)

#define SYLAR_LOG_FATAL(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::FATAL)

#define SYLAR_LOG_NAME(name) sylar::LoggerManager::GetInstance()->getLogger(name)

#define SYLAR_LOG_ROOT() SYLAR_LOG_NAME("root")
namespace sylar
{
    class Logger;
    /**
     * @brief 日志级别
     */
    class LogLevel
    {
    public:
        /**
         * @brief 日志级别枚举
         */
        enum Level
        {
            /// 未知级别
            UNKNOW = 0,
            /// DEBUG 级别
            DEBUG = 1,
            /// INFO 级别
            INFO = 2,
            /// WARN 级别
            WARN = 3,
            /// ERROR 级别
            ERROR = 4,
            /// FATAL 级别
            FATAL = 5
        };

        /**
         * @brief 将日志级别转成文本输出
         * @param[in] level 日志级别
         */
        static const char *ToString(LogLevel::Level level);

        /**
         * @brief 将文本转换成日志级别
         * @param[in] str 日志级别文本
         */
        static LogLevel::Level FromString(const std::string &str);
    };

    class LogEvent
    {
    public:
        typedef std::shared_ptr<LogEvent> ptr;
        /**
         * @brief 构造函数
         * @param[in] logger 日志器
         * @param[in] level 日志级别
         * @param[in] file 文件名
         * @param[in] line 文件行号
         * @param[in] elapse 程序启动依赖的耗时(毫秒)
         * @param[in] thread_id 线程id
         * @param[in] fiber_id 协程id
         * @param[in] time 日志事件(秒)
         * @param[in] thread_name 线程名称
         */
        LogEvent(std::shared_ptr<Logger> logger,
                 LogLevel::Level level,
                 const char *file,
                 int32_t line,
                 uint32_t elapse,
                 uint64_t thread_id,
                 uint64_t fiber_id,
                 uint64_t time,
                 const std::string &thread_name);
        ~LogEvent();

        const char *getFile() const { return m_file; }

        int32_t getLine() const { return m_line; }

        uint32_t getElapse() const { return m_elapse; }

        uint64_t getThreadId() const { return m_threadId; }

        uint64_t getFiberId() const { return m_fiberId; }

        const std::string &getThreadName() const { return m_threadName; }

        std::string getContent() const { return m_ss.str(); }

        LogLevel::Level getLevel() const { return m_level; }

        std::shared_ptr<Logger> getLogger() const { return m_logger; }

        std::stringstream &getSS() { return m_ss; }

        uint64_t getTime() const { return m_time; }

        void format(const char *fmt, ...);

        void format(const char *fmt, va_list al);

    private:
        const char *m_file = nullptr;
        int32_t m_line = 0;
        uint32_t m_elapse = 0;
        uint64_t m_threadId = 0;
        uint64_t m_fiberId = 0;
        uint64_t m_time;
        std::string m_threadName;
        std::stringstream m_ss;
        LogLevel::Level m_level;
        std::shared_ptr<Logger> m_logger;
    };

    /**
     * @brief 日志格式化
     */
    class LogFormatter
    {
    public:
        typedef std::shared_ptr<LogFormatter> ptr;
        /**
         * @brief 构造函数
         * @param[in] pattern 格式模板
         * @details
         *  %m 消息
         *  %p 日志级别
         *  %r 累计毫秒数
         *  %c 日志名称
         *  %t 线程id
         *  %n 换行
         *  %d 时间
         *  %f 文件名
         *  %l 行号
         *  %T 制表符
         *  %F 协程id
         *  %N 线程名称
         *
         *  默认格式 "%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"
         */
        LogFormatter(const std::string &pattern);

        /**
         * @brief 返回格式化日志文本
         * @param[in] event 日志事件
         */
        std::string format(LogEvent::ptr event);
        std::ostream &format(std::ostream &ofs, LogEvent::ptr event);

    public:
        /**
         * @brief 日志内容项格式化
         */
        class FormatItem
        {
        public:
            typedef std::shared_ptr<FormatItem> ptr;
            FormatItem(const std::string &fmt = ""){};
            /**
             * @brief 析构函数
             */
            virtual ~FormatItem() {}
            /**
             * @brief 格式化日志到流
             * @param[in, out] os 日志输出流
             * @param[in] logger 日志器
             * @param[in] level 日志等级
             * @param[in] event 日志事件
             */
            virtual void format(std::ostream &os, LogEvent::ptr event) = 0;
        };

        /**
         * @brief 初始化,解析日志模板
         */
        void init();

        /**
         * @brief 是否有错误
         */
        bool isError() const { return m_error; }

        /**
         * @brief 返回日志模板
         */
        const std::string getPattern() const { return m_pattern; }

    private:
        /// 日志格式模板
        std::string m_pattern;
        /// 日志格式解析后格式
        std::vector<FormatItem::ptr> m_items;
        /// 是否有错误
        bool m_error = false;
    };

    /**
     * @brief 日志输出目标
     */
    class LogAppender
    {
        friend class Logger;

    public:
        typedef std::shared_ptr<LogAppender> ptr;
        typedef Spinlock MutexType;

        /**
         * @brief 析构函数
         */
        virtual ~LogAppender() {}

        /**
         * @brief 写入日志
         * @param[in] logger 日志器
         * @param[in] level 日志级别
         * @param[in] event 日志事件
         */
        virtual void log(LogEvent::ptr event) = 0;

        /**
         * @brief 将日志输出目标的配置转成YAML String
         */
        virtual std::string toYamlString() = 0;

        /**
         * @brief 更改日志格式器
         */
        void setFormatter(LogFormatter::ptr val);

        /**
         * @brief 获取日志格式器
         */
        LogFormatter::ptr getFormatter();

        /**
         * @brief 获取日志级别
         */
        LogLevel::Level getLevel() const { return m_level; }

        /**
         * @brief 设置日志级别
         */
        void setLevel(LogLevel::Level val) { m_level = val; }

    protected:
        /// 日志级别
        LogLevel::Level m_level = LogLevel::Level::DEBUG;
        /// 是否有自己的日志格式器
        bool m_hasFormatter = false;
        /// Mutex
        MutexType m_mutex;
        /// 日志格式器
        LogFormatter::ptr m_formatter;
    };

    class LoggerManager;
    /**
     * @brief 日志器
     */

    class Logger : public std::enable_shared_from_this<Logger>
    {
        friend class LoggerManager;

    public:
        typedef std::shared_ptr<Logger> ptr;
        typedef Spinlock MutexType;

        /**
         * @brief 构造函数
         * @param[in] name 日志器名称
         */
        Logger(const std::string &name = "root");

        /**
         * @brief 写日志
         * @param[in] level 日志级别
         * @param[in] event 日志事件
         */
        void log(LogEvent::ptr event);

        /**
         * @brief 添加日志目标
         * @param[in] appender 日志目标
         */
        void addAppender(LogAppender::ptr appender);

        /**
         * @brief 删除日志目标
         * @param[in] appender 日志目标
         */
        void delAppender(LogAppender::ptr appender);

        /**
         * @brief 清空日志目标
         */
        void clearAppenders();

        /**
         * @brief 返回日志级别
         */
        LogLevel::Level getLevel() const { return m_level; }

        /**
         * @brief 设置日志级别
         */
        void setLevel(LogLevel::Level val) { m_level = val; }

        /**
         * @brief 返回日志名称
         */
        const std::string &getName() const { return m_name; }

        /**
         * @brief 设置日志格式器
         */
        void setFormatter(LogFormatter::ptr val);

        /**
         * @brief 设置日志格式模板
         */
        void setFormatter(const std::string &val);

        /**
         * @brief 获取日志格式器
         */
        LogFormatter::ptr getFormatter();

        /**
         * @brief 将日志器的配置转成YAML String
         */
        std::string toYamlString();

    private:
        /// 日志名称
        std::string m_name;
        /// 日志级别
        LogLevel::Level m_level;
        /// Mutex
        MutexType m_mutex;
        /// 日志目标集合
        std::list<LogAppender::ptr> m_appenders;
        /// 日志格式器
        LogFormatter::ptr m_formatter;
        /// 主日志器
        Logger::ptr m_root;
    };

    /**
     * @brief 输出到控制台的Appender
     */
    class StdoutLogAppender : public LogAppender
    {
    public:
        typedef std::shared_ptr<StdoutLogAppender> ptr;
        void log(LogEvent::ptr event) override;
        std::string toYamlString() override;
    };

    /**
     * @brief 输出到文件的Appender
     */
    class FileLogAppender : public LogAppender
    {
    public:
        typedef std::shared_ptr<FileLogAppender> ptr;
        FileLogAppender(const std::string &filename);
        void log(LogEvent::ptr event) override;
        std::string toYamlString() override;

        /**
         * @brief 重新打开日志文件
         * @return 成功返回true
         */
        bool reopen();

    private:
        /// 文件路径
        std::string m_filename;
        /// 文件流
        std::ofstream m_filestream;
        /// 上次重新打开时间
        uint64_t m_lastTime = 0;
    };

    /**
     * @brief 日志器管理类
     */
    class LoggerManager
    {
    private:
        LoggerManager();

    public:
        typedef Spinlock MutexType;
        typedef std::shared_ptr<LoggerManager> ptr;
        /**
         * @brief 构造函数
         */

        /**
         * @brief 获取日志器
         * @param[in] name 日志器名称
         */
        Logger::ptr getLogger(const std::string &name);

        /**
         * @brief 初始化
         */
        void init();

        /**
         * @brief 返回主日志器
         */
        Logger::ptr getRoot() const { return m_root; }

        static LoggerManager::ptr GetInstance();

    private:
        /// Mutex
        MutexType m_mutex;
        /// 日志器容器
        std::map<std::string, Logger::ptr> m_loggers;
        /// 主日志器
        Logger::ptr m_root;
    };

    /**
     * @brief 日志事件包装器
     */
    class LogEventWrap
    {
    public:
        /**
         * @brief 构造函数
         * @param[in] e 日志事件
         */
        LogEventWrap(LogEvent::ptr e);

        /**
         * @brief 析构函数
         */
        ~LogEventWrap();

        /**
         * @brief 获取日志事件
         */
        LogEvent::ptr getEvent() const { return m_event; }

        /**
         * @brief 获取日志内容流
         */
        std::stringstream &getSS();

    private:
        /**
         * @brief 日志事件
         */
        LogEvent::ptr m_event;
    };
}

#endif