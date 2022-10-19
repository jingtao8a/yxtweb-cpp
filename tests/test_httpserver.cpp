/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-10-18 20:31:57
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-10-19 13:21:55
 * @FilePath: /yxtweb-cpp/tests/test_httpserver.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "../yxtwebcpp/yxtwebcpp.hpp"

static std::shared_ptr<YXTWebCpp::Logger> g_logger = YXTWebCpp_LOG_ROOT();


class MyServlet : public YXTWebCpp::http::Servlet {
public:
    uint64_t handle(std::shared_ptr<YXTWebCpp::http::HttpRequest> req, std::shared_ptr<YXTWebCpp::http::HttpResponse> res, std::shared_ptr<YXTWebCpp::http::HttpStream> stream) {
        res->setBody("I don't know");
        return 0;
    }
};

void testhttpserver() {
    std::shared_ptr<YXTWebCpp::http::HttpServer> server(new YXTWebCpp::http::HttpServer(true));
    auto addr = YXTWebCpp::IPAddress::Create("192.168.159.129", 8999);
    std::vector<std::shared_ptr<YXTWebCpp::Address> > addrs, fail;
    addrs.push_back(addr);
    server->getServletDispatch()->addServlet("/yu", [](std::shared_ptr<YXTWebCpp::http::HttpRequest> req, 
                                                    std::shared_ptr<YXTWebCpp::http::HttpResponse> res, 
                                                    std::shared_ptr<YXTWebCpp::http::HttpStream> stream)->uint64_t {
        res->setBody("Hello, I am yuxintao");
        return 0;
    });
    server->getServletDispatch()->addServlet("/", std::make_shared<MyServlet>());
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