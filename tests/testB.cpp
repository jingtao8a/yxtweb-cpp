/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-10-13 15:26:59
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-10-15 19:21:59
 * @FilePath: /yxtweb-cpp/tests/testB.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "../yxtwebcpp/yxtwebcpp.hpp"

static std::shared_ptr<YXTWebCpp::Logger> g_logger = YXTWebCpp_LOG_ROOT();

void testB() {
    auto sock = YXTWebCpp::Socket::CreateTCPSocket();
    auto remoteAddress = YXTWebCpp::IPAddress::Create("127.0.0.1", 8877);
    sock->connect(remoteAddress);
    YXTWebCpp_LOG_INFO(g_logger) << sock->toString();
    std::string buffer;
    buffer.resize(1024);
    sock->recv((void *)buffer.c_str(), buffer.size());
    YXTWebCpp_LOG_DEBUG(g_logger) << buffer;

    buffer.clear();
    buffer = "hello";
    sock->send((const char *)buffer.c_str(), buffer.size());
}

int main() {
    testB();
    return 0;
}