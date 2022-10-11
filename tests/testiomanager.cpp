/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-10-09 12:57:35
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-10-11 11:40:43
 * @FilePath: /yxtweb-cpp/tests/testiomanager.cpp
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

int sockfd = 0;

static std::shared_ptr<YXTWebCpp::Logger> g_logger = YXTWebCpp_LOG_ROOT();

void test() {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = 80;
    addr.sin_addr.s_addr = inet_addr("14.215.177.38");
    YXTWebCpp_LOG_INFO(g_logger) << "connecting ....";
    connect(sockfd, (sockaddr *)&addr, sizeof(sockaddr));
    YXTWebCpp_LOG_INFO(g_logger) << "connect success";
    YXTWebCpp::IOManager::GetThis()->addEvent(sockfd, YXTWebCpp::IOManager::WRITE, []() {
        YXTWebCpp_LOG_INFO(g_logger) << "write event";
        close(sockfd);
    });
}

void test1() {
    YXTWebCpp::IOManager ioManager(1, true, "test");
    // ioManager.schedule(test);
    test();
}

int main() {
    test1();
    return 0;
}