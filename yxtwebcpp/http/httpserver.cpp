/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-10-18 20:08:38
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-10-18 20:45:42
 * @FilePath: /yxtweb-cpp/yxtwebcpp/http/httpserver.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "httpserver.hpp"
#include "httpstream.hpp"
#include "../log.hpp"

namespace YXTWebCpp {

static std::shared_ptr<Logger> g_logger = YXTWebCpp_LOG_NAME("system");

namespace http {
    
HttpServer::HttpServer(bool keepalive, IOManager* worker, IOManager* accept_worker)
                :TCPServer(worker, accept_worker)
                ,m_keepalive(keepalive) { 
    setName("yuxintao/httpserver");
}

void HttpServer::handleClient(std::shared_ptr<Socket> client) {
    YXTWebCpp_LOG_INFO(g_logger) << "handle client" << *client;
    HttpStream httpStream(client);
    do {
        auto res = httpStream.recvRequest();
        if (res == nullptr) {
            break;
        }
        YXTWebCpp_LOG_INFO(g_logger) << *res;
        std::shared_ptr<HttpResponse> rsp(new HttpResponse(res->getVersion()
                            ,res->isClose() || !m_keepalive));
        rsp->setHeader("Server", getName());
        rsp->setBody("Hello I am yuxintao");
        httpStream.sendResponse(rsp);
        if (!m_keepalive || res->isClose()) {//这是一个短连接
            break;
        }
    } while (true);
}

}

}
