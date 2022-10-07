/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-10-07 13:39:36
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-10-07 22:02:39
 * @FilePath: /yxtweb-cpp/tests/testscheduler.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AEnt
 */
#include "../yxtwebcpp/yxtwebcpp.hpp"
static std::shared_ptr<YXTWebCpp::Logger> g_logger = YXTWebCpp_LOG_ROOT();

void fun1() {
    YXTWebCpp_LOG_DEBUG(g_logger) << "fun1";
}

void fun2() {
    YXTWebCpp_LOG_DEBUG(g_logger) << "fun2";
}

void fun3() {
    YXTWebCpp_LOG_DEBUG(g_logger) << "fun3";
}


void fun4() {
    YXTWebCpp_LOG_DEBUG(g_logger) << "fun4";
}

void fun5() {
    YXTWebCpp_LOG_DEBUG(g_logger) << "fun5";
}

void fun6() {
    YXTWebCpp_LOG_DEBUG(g_logger) << "fun6";
}

int main() {
    YXTWebCpp::Scheduler s(1, false,"test");
    s.schedule(fun1);
    s.schedule(fun2);
    s.schedule(fun3);
    s.schedule(fun4);
    s.schedule(fun5);
    s.schedule(fun6);
    s.start();
    s.stop();
    return 0;
}