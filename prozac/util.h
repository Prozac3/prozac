#ifndef __UTIL_H__
#define __UTIL_H__
#include <vector>
#include <string>
namespace prozac
{
    pid_t GetThreadId();
    uint64_t GetFiberId();

    static std::string demangle(const char *str);
    void Backtrace(std::vector<std::string> &bt, int size = 64, int skip = 1);

    /**
     * @brief 获取当前栈信息的字符串
     * @param[in] size 栈的最大层数
     * @param[in] skip 跳过栈顶的层数
     * @param[in] prefix 栈信息前输出的内容
     */
    std::string BacktraceToString(int size = 64, int skip = 2, const std::string &prefix = "");
    /**
     * @brief 获取当前时间的毫秒
     */
    uint64_t GetCurrentMS();

    /**
     * @brief 获取当前时间的微秒
     */
    uint64_t GetCurrentUS();
    std::string Time2Str(time_t ts = time(0), const std::string &format = "%Y-%m-%d %H:%M:%S");
    time_t Str2Time(const char *str, const char *format = "%Y-%m-%d %H:%M:%S");
}

#endif