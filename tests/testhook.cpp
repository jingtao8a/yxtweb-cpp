/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-10-11 12:50:06
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-10-12 21:54:00
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
    addr.sin_family = AF_INET;
    addr.sin_port = 80;
    addr.sin_addr.s_addr = inet_addr("14.215.177.38");
    YXTWebCpp_LOG_INFO(g_logger) << "connecting ....";
    int rt = connect(sockfd, (sockaddr *)&addr, sizeof(sockaddr));
    YXTWebCpp_LOG_INFO(g_logger) << "rt = " << rt;
    // YXTWebCpp::IOManager::GetThis()->addEvent(sockfd, YXTWebCpp::IOManager::WRITE, []() {
    //     YXTWebCpp_LOG_INFO(g_logger) << "write event";
    //     close(sockfd);
    // });
}

int main() {
    // test_sleep();
    //YXTWebCpp::IOManager iom;
    //iom.schedule(test_socket);
    test_socket();
    return 0;
}
