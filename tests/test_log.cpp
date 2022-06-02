#include <prozac/log.h>
#include <prozac/fiber.h>
#include <prozac/util.h>
#include <prozac/thread.h>
int main(int argc, char **argv)
{
    prozac::Thread::SetName("main");
    auto logger = PROZAC_LOG_NAME("system");
    PROZAC_LOG_LEVEL(logger,prozac::LogLevel::INFO)<<"nmsl";
    PROZAC_LOG_ERROR(logger) << "gg";
    return 0;
}