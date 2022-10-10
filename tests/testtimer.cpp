/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-10-10 15:35:32
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-10-10 20:30:28
 * @FilePath: /yxtweb-cpp/tests/testtimer.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "../yxtwebcpp/yxtwebcpp.hpp"

static std::shared_ptr<YXTWebCpp::Logger> g_logger = YXTWebCpp_LOG_ROOT();
std::shared_ptr<YXTWebCpp::Timer> s_timer;

void test_timer() {
    YXTWebCpp::IOManager iom(1, true, "test");    
    s_timer = iom.addTimer(1000, []() {
        static int i = 0;
        YXTWebCpp_LOG_INFO(g_logger) << "hello timer i = " << i;
        if (++i == 10) {
            s_timer->cancel();
        }
    }, true);
}

int main() {
    test_timer();
    return 0;
}