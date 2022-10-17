/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-10-17 11:18:47
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-10-17 11:48:10
 * @FilePath: /yxtweb-cpp/tests/testhttp.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AEn
 */
#include "../yxtwebcpp/yxtwebcpp.hpp"

std::shared_ptr<YXTWebCpp::Logger> g_logger = YXTWebCpp_LOG_ROOT();

void testrequest() {
    YXTWebCpp::http::HttpRequest request;
    request.setMethod(YXTWebCpp::http::HttpMethod::GET);
    request.setVersion(0X11);
    request.setClose(false);
    request.setHeader("host", "www.baidu.com");
    request.setBody("hello");
    std::cout << request;
}

void testresponse() {
    YXTWebCpp::http::HttpResponse response;
    response.setVersion(0x11);
    response.setStatus(YXTWebCpp::http::HttpStatus::OK);
    response.setHeader("X-X", "yuxintao");
    response.setBody("hello, I am yuxintao");
    std::cout << response;
}

int main() {
    testrequest();
    std::cout << std::endl;
    testresponse();
    std::cout << std::endl;
    return 0;
}
