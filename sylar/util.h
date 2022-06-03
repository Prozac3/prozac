#ifndef __UTIL_H__
#define __UTIL_H__
#include <vector>
#include <string>
#include <iostream>
#include <cxxabi.h>
namespace sylar
{
    /**
     * @brief 获取线程ID
     *
     * @return pid_t    线程ID
     */
    pid_t GetThreadId();

    /**
     * @brief 获取协程ID
     *
     * @return uint64_t 协程ID
     */
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
    class FSUtil
    {
    public:
        static void ListAllFile(std::vector<std::string> &files, const std::string &path, const std::string &subfix);
        static bool Mkdir(const std::string &dirname);
        static bool IsRunningPidfile(const std::string &pidfile);
        static bool Rm(const std::string &path);
        static bool Mv(const std::string &from, const std::string &to);
        static bool Realpath(const std::string &path, std::string &rpath);
        static bool Symlink(const std::string &frm, const std::string &to);
        static bool Unlink(const std::string &filename, bool exist = false);
        static std::string Dirname(const std::string &filename);
        static std::string Basename(const std::string &filename);
        static bool OpenForRead(std::ifstream &ifs, const std::string &filename, std::ios_base::openmode mode);
        static bool OpenForWrite(std::ofstream &ofs, const std::string &filename, std::ios_base::openmode mode);
    };

    template <class T>
    const char *TypeToName()
    {
        static const char *s_name = abi::__cxa_demangle(typeid(T).name(), nullptr, nullptr, nullptr);
        return s_name;
    }
}

#endif