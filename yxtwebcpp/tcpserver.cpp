/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-10-18 11:00:50
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-10-18 13:24:17
 * @FilePath: /yxtweb-cpp/yxtwebcpp/tcpserver.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "tcpserver.hpp"
#include "log.hpp"
#include "config.hpp"

namespace YXTWebCpp {

static std::shared_ptr<Logger> g_logger = YXTWebCpp_LOG_NAME("system");
static std::shared_ptr<ConfigVar<uint64_t> >  g_tcp_server_read_timeout = Config::Lookup("tcp_server.read_timeout",
                (uint64_t)(5 * 1000), "tcp server read timeout");

TCPServer::TCPServer(IOManager* worker, IOManager* accept_worker) 
            :m_worker(worker)
            ,m_accept_worker(accept_worker) 
            ,m_name("yuxintao/tcpserver") 
            ,m_recvTimeout(g_tcp_server_read_timeout->getValue()) {
}

TCPServer::~TCPServer() {
    for (auto& i : m_sockets) {
        i->close();
    }
    m_sockets.clear();
}

bool TCPServer::bind(std::shared_ptr<Address> addr) {
    std::vector<std::shared_ptr<Address> > addrs;
    std::vector<std::shared_ptr<Address> > fails;
    addrs.push_back(addr);
    return bind(addrs, fails);
}

bool TCPServer::bind(std::vector<std::shared_ptr<Address> > addrs, std::vector<std::shared_ptr<Address> > failaddrs) {
    for (auto& addr : addrs) {
        std::shared_ptr<Socket> sock = Socket::CreateTCP(addr);
        if (!sock->bind(addr)) {
            YXTWebCpp_LOG_ERROR(g_logger) << "bind fail errno="
                << errno << " errstr=" << strerror(errno)
                << " addr=[" << addr->toString() << "]";
            failaddrs.push_back(addr);
            continue;
        }
        if (!sock->listen()) {
            YXTWebCpp_LOG_ERROR(g_logger) << "listen fail errno="
                << errno << " errstr=" << strerror(errno)
                << " addr=[" << addr->toString() << "]";
            failaddrs.push_back(addr);
            continue;
        }
        m_sockets.push_back(sock);
    }
    if (!failaddrs.empty()) {
        m_sockets.clear();
        return false;
    }
    for (auto& i : m_sockets) {
        YXTWebCpp_LOG_INFO(g_logger)
            << " name=" << m_name
            << " server bind success: " << i->toString();
    }
    return true;
}

void TCPServer::start() {
    if (!isStop) {
        return;
    }
    isStop = false;
    for (auto& sockptr : m_sockets) {
        m_accept_worker->schedule(std::bind(&TCPServer::startAccept, shared_from_this(), sockptr));
    }
}

void TCPServer::stop() {
    isStop = true;
    auto self = shared_from_this();
    m_accept_worker->schedule([self]() {
        for (auto& sock : self->m_sockets) {
            sock->cancelAll();
            sock->close();
        }
        self->m_sockets.clear();
    });
}

void TCPServer::startAccept(std::shared_ptr<Socket> sock) {
    YXTWebCpp_LOG_DEBUG(g_logger) << "start accept";
    while (!isStop) {
        auto clientSock = sock->accept();//如果阻塞，往IOManger中加入监听事件，并且挂起该协程
        if (clientSock) {
            m_worker->schedule(std::bind(&TCPServer::handleClient, shared_from_this(), clientSock));
        }
    }
}

void TCPServer::handleClient(std::shared_ptr<Socket> sock) {
    YXTWebCpp_LOG_INFO(g_logger) << "handle client" << sock->toString();
    sock->setRecvTimeout(m_recvTimeout);
    while (!isStop) {
        std::string buffer;
        buffer.resize(1024);
        int rt = sock->recv((void*)&buffer[0], buffer.size(), 0);
        if (rt > 0) {
            YXTWebCpp_LOG_INFO(g_logger) << buffer;
        } else {
            break;
        }
    }
}

}