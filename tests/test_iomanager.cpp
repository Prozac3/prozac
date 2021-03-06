#include <sylar/iomanager.h>
#include <sylar/log.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <sys/epoll.h>

sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

int sock = 0;

void test_fiber()
{
    SYLAR_LOG_INFO(g_logger) << "test_fiber sock=" << sock;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(sock, F_SETFL, O_NONBLOCK);

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(6000);
    inet_pton(AF_INET, "192.168.1.105", &addr.sin_addr.s_addr);

    if (!connect(sock, (const sockaddr *)&addr, sizeof(addr)))
    {
    }
    else if (errno == EINPROGRESS)
    {
        SYLAR_LOG_INFO(g_logger) << "add event errno=" << errno << " " << strerror(errno);
        sylar::IOManager::GetThis()->addEvent(sock, sylar::IOManager::READ, []()
                                              { SYLAR_LOG_INFO(g_logger) << "read callback"; });
        sylar::IOManager::GetThis()->addEvent(sock, sylar::IOManager::WRITE, []()
                                              {
            SYLAR_LOG_INFO(g_logger) << "write callback";
         });
    }
    else
    {
        SYLAR_LOG_INFO(g_logger) << "else " << errno << " " << strerror(errno);
    }
}

void test1()
{
    std::cout << "EPOLLIN=" << EPOLLIN
              << " EPOLLOUT=" << EPOLLOUT << std::endl;
    sylar::IOManager iom(8, "iom_test");
    iom.submit(sylar::Fiber::ptr(sylar::Fiber::CreatFiber(test_fiber)));
    sleep(4);
}

int main(int argc, char **argv)
{
    test1();
    return 0;
}