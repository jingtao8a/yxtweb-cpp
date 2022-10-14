/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-10-14 18:56:20
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-10-14 20:30:56
 * @FilePath: /yxtweb-cpp/tests/testaddress.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "../yxtwebcpp/yxtwebcpp.hpp"
#include <vector>

static std::shared_ptr<YXTWebCpp::Logger> g_logger = YXTWebCpp_LOG_ROOT();

void test() {
    std::vector<std::shared_ptr<YXTWebCpp::Address> > addrs;
    bool v = YXTWebCpp::Address::Lookup(addrs, "www.baidu.com");
    if (!v) {
        YXTWebCpp_LOG_ERROR(g_logger) << "lookup fail";
        return;
    }
    for (size_t i = 0; i < addrs.size(); ++i) {
        YXTWebCpp_LOG_INFO(g_logger) << i << " - " << addrs[i]->toString();
    }
}


void test2() {
    std::multimap<std::string, std::pair<std::shared_ptr<YXTWebCpp::Address>, uint32_t> > result;
    bool v = YXTWebCpp::Address::GetInterfaceAddresses(result);
    if (!v) {
        YXTWebCpp_LOG_ERROR(g_logger) << "GetInerfaceAddresses fail";
        return;
    }
    for (auto i : result) {
        YXTWebCpp_LOG_INFO(g_logger) << i.first << " " << i.second.first->toString() << " " << i.second.second;
    }
}

void test3() {
    auto addr = YXTWebCpp::IPAddress::Create("78:1:1:1:2:2:2:3");
    YXTWebCpp_LOG_DEBUG(g_logger) << addr->toString();
}

int main() {
    test3();
    return 0;
}