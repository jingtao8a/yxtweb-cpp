/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-09-17 16:37:43
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-10-05 23:07:19
 * @FilePath: /YXTWebCpp/tests/test.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "../yxtwebcpp/yxtwebcpp.hpp"

int main() {
    // std::shared_ptr<YXTWebCpp::Logger> logger(new YXTWebCpp::Logger());
    // std::shared_ptr<YXTWebCpp::FileLogAppender> fileLogAppender(new YXTWebCpp::FileLogAppender("./logtxt.txt"));
    // logger->addAppender(fileLogAppender);
    // std::shared_ptr<YXTWebCpp::LogEvent> event = std::make_shared<YXTWebCpp::LogEvent>(logger, YXTWebCpp::LogLevel::Level::DEBUG, "file", 0, 0, 0, 0, 0, "thread");
    // event->format("haha%d", 100);
    // logger->log(event->getLevel(), event);
    void *ptr = malloc(128 * 1024);
    free(ptr);
    return 0;
}