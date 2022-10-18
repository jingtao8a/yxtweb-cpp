/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-10-18 20:31:57
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-10-18 20:40:11
 * @FilePath: /yxtweb-cpp/tests/test_httpserver.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "../yxtwebcpp/yxtwebcpp.hpp"

static std::shared_ptr<YXTWebCpp::Logger> g_logger = YXTWebCpp_LOG_ROOT();

void testhttpserver() {
    std::shared_ptr<YXTWebCpp::http::HttpServer> server(new YXTWebCpp::http::HttpServer(true));
    auto addr = YXTWebCpp::IPAddress::Create("192.168.159.129", 8999);
    std::vector<std::shared_ptr<YXTWebCpp::Address> > addrs, fail;
    addrs.push_back(addr);
    while (!server->bind(addrs, fail)) {
        sleep(2);
        YXTWebCpp_LOG_DEBUG(g_logger) << "bind again";
    }
    server->start();
}

int main() {
    YXTWebCpp::IOManager iom;
    iom.schedule(testhttpserver);
    return 0;
}