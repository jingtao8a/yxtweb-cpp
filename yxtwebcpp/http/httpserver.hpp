/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-10-18 20:08:45
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-10-18 20:30:28
 * @FilePath: /yxtweb-cpp/yxtwebcpp/httpserver.hpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef _HTTPSERVER_HPP_
#define _HTTPSERVER_HPP_
#include "../tcpserver.hpp"

namespace YXTWebCpp {

namespace http {

class HttpServer : public TCPServer {
public:
    HttpServer(bool keepalive = false
                ,IOManager* worker = IOManager::GetThis()
                ,IOManager* accept_worker = IOManager::GetThis());
protected:
    void handleClient(std::shared_ptr<Socket> client) override;
private:
    bool m_keepalive;//表示该服务器是否支持connection: keepalive
};

}

}

#endif