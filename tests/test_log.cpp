#include <prozac/log.h>
#include <prozac/fiber.h>
#include <prozac/util.h>
#include <prozac/thread.h>
int main(int argc, char **argv)
{
    prozac::Thread::SetName("main");
    auto logger = PROZAC_LOG_NAME("root");
    PROZAC_LOG_LEVEL(logger, prozac::LogLevel::INFO) << "This is INFO!";
    PROZAC_LOG_ERROR(logger) << "This is ERROR!";
    PROZAC_LOG_FATAL(logger) << "This is FATAL!";
    return 0;
}