/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-10-05 20:48:34
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-10-05 23:42:21
 * @FilePath: /yxtweb-cpp/tests/testfiber.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "../yxtwebcpp/yxtwebcpp.hpp"

std::shared_ptr<YXTWebCpp::Logger> g_logger = YXTWebCpp_LOG_ROOT();

void fun() {
    YXTWebCpp_LOG_DEBUG(g_logger) << "fun begin";
    YXTWebCpp::Fiber::YieldToHold();
    YXTWebCpp_LOG_DEBUG(g_logger) << "fun execute";
    YXTWebCpp::Fiber::YieldToHold();
    YXTWebCpp_LOG_DEBUG(g_logger) << "fun end";
}

int main() {
    YXTWebCpp::Thread::SetName("main");
    std::function<void()> cb(fun); 
    std::shared_ptr<YXTWebCpp::Fiber> fiberptr(new YXTWebCpp::Fiber(cb));
    YXTWebCpp_LOG_DEBUG(g_logger) << "main begin";
    YXTWebCpp::Fiber::GetThis();
    fiberptr->swapIn();
    YXTWebCpp_LOG_DEBUG(g_logger) << "main execute";
    fiberptr->swapIn();
    YXTWebCpp_LOG_DEBUG(g_logger) << "main execute 2";
    fiberptr->swapIn();
    YXTWebCpp_LOG_DEBUG(g_logger) << "main end";
    return 0;
}