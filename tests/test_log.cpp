#include <sylar/log.h>
#include <sylar/fiber.h>
#include <sylar/util.h>
#include <sylar/thread.h>
int main(int argc, char **argv)
{
    sylar::Thread::SetName("main");
    auto logger = SYLAR_LOG_NAME("root");
    SYLAR_LOG_LEVEL(logger, sylar::LogLevel::INFO) << "This is INFO!";
    SYLAR_LOG_ERROR(logger) << "This is ERROR!";
    SYLAR_LOG_FATAL(logger) << "This is FATAL!";
    return 0;
}