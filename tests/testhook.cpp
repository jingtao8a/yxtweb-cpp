/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-10-11 12:50:06
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-10-13 20:45:00
 * @FilePath: /yxtweb-cpp/tests/testhook.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */

#include "../yxtwebcpp/yxtwebcpp.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <netdb.h>

static std::shared_ptr<YXTWebCpp::Logger> g_logger = YXTWebCpp_LOG_ROOT();

void test_sleep() {
    YXTWebCpp::IOManager iom(1, true, "test");
    iom.schedule([]() {
        YXTWebCpp_LOG_INFO(g_logger) << "sleep 2 before";
        sleep(2);
        YXTWebCpp_LOG_INFO(g_logger) << "sleep 2";
    });

    iom.schedule([]() {
        YXTWebCpp_LOG_INFO(g_logger) << "sleep 3 before";
        sleep(3);
        YXTWebCpp_LOG_INFO(g_logger) << "sleep 3";
    });

    YXTWebCpp_LOG_INFO(g_logger) << "test_sleep";
}

int sockfd = 0;

void test_socket() {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8888);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    YXTWebCpp_LOG_INFO(g_logger) << "connecting ....";
    int rt = connect(sockfd, (sockaddr *)&addr, sizeof(addr));
    if (rt == -1) {
        YXTWebCpp_LOG_INFO(g_logger) << "connect rt = " << rt << " errno = " << errno;
        return;
    }
    YXTWebCpp::IOManager::GetThis()->addEvent(sockfd, YXTWebCpp::IOManager::WRITE, []() {
        YXTWebCpp_LOG_INFO(g_logger) << "write event";
        const char data[] = "GET /1 HTTP/1.1\r\n\r\n";
        auto sockfdContext = YXTWebCpp::FdMgr::GetInstance()->get(sockfd);
        sockfdContext->setTimeout(SO_SNDTIMEO, 3000);
        sockfdContext->setTimeout(SO_RCVTIMEO, 3000);

        int rt = send(sockfd, data, sizeof(data), 0);
        if (rt <= 0) {
            YXTWebCpp_LOG_INFO(g_logger) << "send rt=" << rt << " errno=" << errno;
            close(sockfd);
            return;
        }
        YXTWebCpp_LOG_DEBUG(g_logger) << "send finish";

        std::string buff;
        buff.resize(4096);

        rt = recv(sockfd, &buff[0], buff.size(), 0);
        
        if (rt <= 0) {
            YXTWebCpp_LOG_INFO(g_logger) << "recv rt=" << rt << " errno=" << errno;
            close(sockfd);
            return;
        }
        
        buff.resize(rt);
        YXTWebCpp_LOG_INFO(g_logger) << buff;
    });
}

void test() {
    for (int i = 0; i < 10; ++i) {
        YXTWebCpp_LOG_DEBUG(g_logger) << "*****";
        sleep(1);
    }
}

int main() {
    // test_sleep();
    YXTWebCpp::IOManager iom;
    iom.schedule(test_socket);
    iom.schedule(test);
    return 0;
}
