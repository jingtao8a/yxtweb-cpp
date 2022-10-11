/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-10-11 12:50:06
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-10-11 13:42:36
 * @FilePath: /yxtweb-cpp/tests/testhook.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */

#include "../yxtwebcpp/yxtwebcpp.hpp"

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

int main() {
    test_sleep();
    return 0;
}