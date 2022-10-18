/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-10-18 12:26:24
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-10-18 13:50:13
 * @FilePath: /yxtweb-cpp/tests/test_tcpserver.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "../yxtwebcpp/yxtwebcpp.hpp"

std::shared_ptr<YXTWebCpp::Logger> g_logger = YXTWebCpp_LOG_ROOT();

void test() {
    std::shared_ptr<YXTWebCpp::Address> addr1 = YXTWebCpp::IPAddress::Create("192.168.159.129", 8999);
    std::shared_ptr<YXTWebCpp::Address> addr2 = YXTWebCpp::IPAddress::Create("192.168.159.129", 8777);
    std::shared_ptr<YXTWebCpp::TCPServer> tcpserver = std::make_shared<YXTWebCpp::TCPServer>();
    std::vector<std::shared_ptr<YXTWebCpp::Address> > addrs, fails;
    addrs.push_back(addr1);
    addrs.push_back(addr2);
    while (!tcpserver->bind(addrs, fails)) {
        sleep(1);
        YXTWebCpp_LOG_DEBUG(g_logger) << "bind again";
    }
    tcpserver->start();
}

int main() {
    YXTWebCpp::IOManager iom;
    iom.schedule(test);
    return 0;
}