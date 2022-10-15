/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-10-13 15:26:52
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-10-15 19:30:06
 * @FilePath: /yxtweb-cpp/tests/testA.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "../yxtwebcpp/yxtwebcpp.hpp"

static std::shared_ptr<YXTWebCpp::Logger> g_logger = YXTWebCpp_LOG_ROOT();

void testA() {
    auto sock = YXTWebCpp::Socket::CreateTCPSocket();
    auto localAddress = YXTWebCpp::IPAddress::Create("127.0.0.1", 8877);
    sock->bind(localAddress);
    sock->listen(5);
    YXTWebCpp_LOG_DEBUG(g_logger) << sock->toString();
    auto newsock = sock->accept();
    YXTWebCpp_LOG_INFO(g_logger) << newsock->toString();
    std::string buffer("yuxintao");
    newsock->send((const void *)buffer.c_str(), buffer.size());

    buffer.clear();
    buffer.resize(1024);
    int rt = newsock->recv((void*)buffer.c_str(), buffer.size());
    YXTWebCpp_LOG_INFO(g_logger) << buffer;
}

int main() {
    testA();
    return 0;
}