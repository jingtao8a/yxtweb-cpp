/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-09-17 16:37:43
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-10-07 22:04:39
 * @FilePath: /YXTWebCpp/tests/test.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "../yxtwebcpp/yxtwebcpp.hpp"

static bool flag = true;

static std::shared_ptr<YXTWebCpp::Logger> g_logger = YXTWebCpp_LOG_ROOT();

void fun1() {
    YXTWebCpp_LOG_DEBUG(g_logger) << "fun1";
}


void thread1() {
    while (true) {
        YXTWebCpp::Fiber::GetThis();
        std::shared_ptr<YXTWebCpp::Fiber> p1(new YXTWebCpp::Fiber(fun1));
        sleep(1);
        p1->swapIn();
        sleep(1);
        p1->swapIn();
    }
    YXTWebCpp_LOG_DEBUG(g_logger) << "end";
}

int main() {
    // std::shared_ptr<YXTWebCpp::Logger> logger(new YXTWebCpp::Logger());
    // std::shared_ptr<YXTWebCpp::FileLogAppender> fileLogAppender(new YXTWebCpp::FileLogAppender("./logtxt.txt"));
    // logger->addAppender(fileLogAppender);
    // std::shared_ptr<YXTWebCpp::LogEvent> event = std::make_shared<YXTWebCpp::LogEvent>(logger, YXTWebCpp::LogLevel::Level::DEBUG, "file", 0, 0, 0, 0, 0, "thread");
    // event->format("haha%d", 100);
    // logger->log(event->getLevel(), event);
    
    YXTWebCpp::Thread t(thread1, "test");
    t.join();
    return 0;
}