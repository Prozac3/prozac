#include <prozac/log.h>
#include <prozac/fiber.h>
#include <prozac/util.h>
#include <prozac/thread.h>
int main(int argc, char **argv)
{
    prozac::Logger::ptr log(new prozac::Logger());
    prozac::StdoutLogAppender::ptr stdApp(new prozac::StdoutLogAppender);
    prozac::LogEvent::ptr event(new prozac::LogEvent(
        log, prozac::LogLevel::INFO,
        __FILE__, __LINE__, 0,
        prozac::GetThreadId(),
        prozac::GetFiberId(),
        prozac::GetCurrentMS(),
        prozac::Thread::GetName()));
    log->addAppender(stdApp);
    log->log(event);
    return 0;
}