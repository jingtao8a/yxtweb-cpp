/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-10-17 18:30:54
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-10-17 19:08:56
 * @FilePath: /yxtweb-cpp/tests/test_http_parser.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "../yxtwebcpp/yxtwebcpp.hpp"

std::shared_ptr<YXTWebCpp::Logger> g_logger = YXTWebCpp_LOG_ROOT();

const char test_request_data[]  = "POST / HTTP/1.1\r\nContent-Length: 10\r\n\r\n1234567890";
const char test_response_data[] = "HTTP/1.0 200 ok \r\n"
                                "Content-Length: 5\r\n\r\n"
                                "hello";

void test1() {
    YXTWebCpp::http::HttpRequestParser requestParser;
    std::string tmp(test_request_data);
    size_t s = requestParser.execute(&tmp[0], tmp.size());
    YXTWebCpp_LOG_INFO(g_logger) << "execute rt = " << s << "has_error = " << requestParser.hasError()
        << "is_finished = " << requestParser.isFinished();
    if (requestParser.hasError()) {
        return;
    }
    YXTWebCpp_LOG_INFO(g_logger) << requestParser.getData()->toString() << tmp.substr(0, requestParser.getContentLength());
}

void test2() {
    YXTWebCpp::http::HttpResponseParser responseParser;
    std::string tmp(test_response_data);
    size_t s = responseParser.execute(&tmp[0], tmp.size(), false);
    YXTWebCpp_LOG_INFO(g_logger) << "execute rt = " << s << " has_error = " << responseParser.hasError()
        << "is_finished = " << responseParser.isFinished();
    if (responseParser.hasError()) {
        return;
    }
    YXTWebCpp_LOG_INFO(g_logger) << responseParser.getData()->toString() << tmp.substr(0, responseParser.getContentLength());
}

int main() {
    test1();
    test2();
    return 0;
}