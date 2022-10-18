/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-10-15 11:03:50
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-10-18 19:29:47
 * @FilePath: /yxtweb-cpp/yxtwebcpp/socket.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */

#include "socket.hpp"
#include "log.hpp"
#include "fd_manager.hpp"
#include "macro.hpp"
#include "hook.hpp"
#include "iomanager.hpp"
#include <netinet/tcp.h>



namespace YXTWebCpp {

static std::shared_ptr<Logger> g_logger = YXTWebCpp_LOG_NAME("system");

//静态

std::shared_ptr<Socket> Socket::CreateTCP(std::shared_ptr<Address> address) {
    std::shared_ptr<Socket> sock(new Socket(address->getFamily(), TCP, 0));
    return sock;
}

std::shared_ptr<Socket> Socket::CreateUDP(std::shared_ptr<Address> address) {
    std::shared_ptr<Socket> sock(new Socket(address->getFamily(), UDP, 0));
    sock->newSock();
    sock->m_isConnected = true;
    return sock;
}

std::shared_ptr<Socket> Socket::CreateTCPSocket() {
    std::shared_ptr<Socket> result = std::make_shared<Socket>(IPv4, TCP, 0);
    return result;
}

std::shared_ptr<Socket> Socket::CreateUDPSocket() {
    std::shared_ptr<Socket> result = std::make_shared<Socket>(IPv4, UDP, 0);
    result->newSock();
    return result;
}

std::shared_ptr<Socket> Socket::CreateTCPSocket6() {
    std::shared_ptr<Socket> result = std::make_shared<Socket>(IPv4, TCP, 0);
    return result;
}

std::shared_ptr<Socket> Socket::CreateUDPSocket6() {
    std::shared_ptr<Socket> result = std::make_shared<Socket>(IPv6, UDP, 0);
    result->newSock();
    return result;
}


std::shared_ptr<Socket> Socket::CreateUnixTCPSocket() {
    std::shared_ptr<Socket> result = std::make_shared<Socket>(UNIX, TCP, 0);
    return result;
}

std::shared_ptr<Socket> Socket::CreateUnixUDPSocket() {
    std::shared_ptr<Socket> result = std::make_shared<Socket>(UNIX, UDP, 0);
    return result;
}

//非静态
Socket::Socket(int family, int type, int protocol) 
        :m_family(family) 
        ,m_type(type) 
        ,m_protocol(protocol)
        ,m_isConnected(false) {
}

Socket::~Socket() {
    close();
}

uint64_t Socket::getSendTimeout() {
    auto fdContext = FdMgr::GetInstance()->get(m_sock);
    if (fdContext) {
        return fdContext->getTimeout(SO_SNDTIMEO);
    }
    return -1;
}

void Socket::setSendTimeout(uint64_t v) {
    auto fdContext = FdMgr::GetInstance()->get(m_sock);
    if (fdContext) {
        fdContext->setTimeout(SO_SNDTIMEO, v);
    }
}

uint64_t Socket::getRecvTimeout() {
    auto fdContext = FdMgr::GetInstance()->get(m_sock);
    if (fdContext) {
        return fdContext->getTimeout(SO_RCVTIMEO);
    }
    return -1;
}

void Socket::setRecvTimeout(uint64_t v) {
    auto fdContext = FdMgr::GetInstance()->get(m_sock);
    if (fdContext) {
        fdContext->setTimeout(SO_RCVTIMEO, v);
    }
}

bool Socket::getOption(int level, int option, void* result, socklen_t* len) {
    int rt = getsockopt(m_sock, level, option, result, (socklen_t*)len);
    if(rt) {
        YXTWebCpp_LOG_DEBUG(g_logger) << "getOption sock=" << m_sock
            << " level=" << level << " option=" << option
            << " errno=" << errno << " errstr=" << strerror(errno);
        return false;
    }
    return true;
}

bool Socket::setOption(int level, int option, const void* result, socklen_t len) {
    int rt = setsockopt(m_sock, level, option, result, (socklen_t)len);
    if (rt) {
        YXTWebCpp_LOG_DEBUG(g_logger) << "setOption sock=" << m_sock
            << " level=" << level << " option=" << option
            << " errno=" << errno << " errstr=" << strerror(errno);
        return false;
    }
    return true;
}

bool Socket::bind(const std::shared_ptr<Address> addr) {
    if (!isValid()) {
        newSock();//如果还没有创建sockfd，如果是TCP sockfd情况
        YXTWebCpp_ASSERT(isValid());
    }

    if (addr->getFamily() != m_family) {//绑定的地址的协议族和sockfd的不一致
        YXTWebCpp_LOG_ERROR(g_logger) << "bind sock.family("
            << m_family << ") addr.family(" << addr->getFamily()
            << ") not equal, addr=" << addr->toString();
        return false;
    }
    // std::shared_ptr<UnixAddress> uaddr = std::dynamic_pointer_cast<UnixAddress>(addr);
    // if (uaddr) {//如果是一个Unix address

    // }
    int rt = ::bind(m_sock, addr->getAddr(),addr->getAddrLen());
    if (rt) {
        YXTWebCpp_LOG_ERROR(g_logger) << "bind error errrno=" << errno
            << " errstr=" << strerror(errno);
        return false;
    }
    getLocalAddress();//TCP socketfd bind后可以获取本地address
    return true;
}

bool Socket::listen(int backlog) {
    if(!isValid()) {
        YXTWebCpp_LOG_ERROR(g_logger) << "listen error sock=-1";
        return false;
    }
    if(::listen(m_sock, backlog)) {
        YXTWebCpp_LOG_ERROR(g_logger) << "listen error errno=" << errno
            << " errstr=" << strerror(errno);
        return false;
    }
    return true;
}

std::shared_ptr<Socket> Socket::accept() {
    std::shared_ptr<Socket> sock(new Socket(m_family, m_type, m_protocol));
    int newsock = ::accept(m_sock, nullptr, nullptr);//newsock是已经connect的TCP sockfd
    if (newsock == -1) {
        YXTWebCpp_LOG_ERROR(g_logger) << "accept(" << m_sock << ") errno="
            << errno << " errstr=" << strerror(errno);
        return nullptr;
    }
    if (!is_hook_enable()) {
        int flags = fcntl(newsock, F_GETFL, 0);
        fcntl(newsock, F_SETFL, flags & ~O_NONBLOCK);//设置为阻塞状态
    }
    if (sock->init(newsock)) {
        return sock;
    }
    return nullptr;
}

bool Socket::close() {
    if (!m_isConnected && m_sock == -1) {
        return true;
    }
    m_isConnected = false;
    if (m_sock != -1) {
        ::close(m_sock);
        m_sock = -1;
        return true;
    }
    return false;
}

bool Socket::connect(const std::shared_ptr<Address> addr, uint64_t timeout_ms) {
    m_remoteAddress = addr;
    if (!isValid()) {
        newSock();//如果是TCP sockfd 需要创建
        YXTWebCpp_ASSERT(isValid());
    }
    if (addr->getFamily() != m_family) {//connect的地址的协议族和sockfd的不一致
        YXTWebCpp_LOG_ERROR(g_logger) << "bind sock.family("
            << m_family << ") addr.family(" << addr->getFamily()
            << ") not equal, addr=" << addr->toString();
        return false;
    }

    if (timeout_ms == (uint64_t)-1) {
        if (::connect(m_sock, addr->getAddr(), addr->getAddrLen())) {
            YXTWebCpp_LOG_ERROR(g_logger) << "sock=" << m_sock << " connect(" << addr->toString()
                << ") error errno=" << errno << " errstr=" << strerror(errno);
            close();
            return false;
        }
    } else {
        if (::connect_with_timeout(m_sock, addr->getAddr(), addr->getAddrLen(), timeout_ms)) {
            YXTWebCpp_LOG_ERROR(g_logger) << "sock=" << m_sock << " connect(" << addr->toString()
                << ") timeout=" << timeout_ms << " error errno="
                << errno << " errstr=" << strerror(errno);
            close();
            return false;
        }
    }
    m_isConnected = true;
    getRemoteAddress();//connect后的TCP 和 UDP sockfd都可以获得本地和远端 address
    getLocalAddress();
    return true;
}

bool Socket::reconnect(uint64_t timeout_ms) {
    if (!m_remoteAddress) {
        YXTWebCpp_LOG_ERROR(g_logger) << "reconnect m_remoteAddress is null";
        return false;
    }
    m_localAddress.reset();
    return connect(m_remoteAddress, timeout_ms);
}

int Socket::send(const void* buffer, size_t length, int flags) {
    if(isConnected()) {
        return ::send(m_sock, buffer, length, flags);
    }
    return -1;
}

int Socket::send(const iovec*buffers, size_t length, int flags) {
    if(isConnected()) {
        msghdr msg;
        memset(&msg, 0, sizeof(msg));
        msg.msg_iov = (iovec*)buffers;
        msg.msg_iovlen = length;
        return ::sendmsg(m_sock, &msg, flags);
    }
    return -1;
}

int Socket::sendTo(const void* buffer, size_t length, const std::shared_ptr<Address> to, int flags) {
    m_remoteAddress = to;
    return ::sendto(m_sock, buffer, length, flags, to->getAddr(), to->getAddrLen());
}

int Socket::sendTo(const iovec* buffers, size_t length, const std::shared_ptr<Address> to, int flags) {
    m_remoteAddress = to;
    msghdr msg;
    memset(&msg, 0, sizeof(msg));
    msg.msg_iov = (iovec*)buffers;
    msg.msg_iovlen = length;
    msg.msg_name = to->getAddr();
    msg.msg_namelen = to->getAddrLen();
    return ::sendmsg(m_sock, &msg, flags);
}

int Socket::recv(void *buffer, size_t length, int flags) {
    if(isConnected()) {
        return ::recv(m_sock, buffer, length, flags);
    }
    return -1;
}

int Socket::recv(iovec* buffers, size_t length, int flags) {
    if(isConnected()) {
        msghdr msg;
        memset(&msg, 0, sizeof(msg));
        msg.msg_iov = (iovec*)buffers;
        msg.msg_iovlen = length;
        return ::recvmsg(m_sock, &msg, flags);
    }
    return -1;
}

int Socket::recvFrom(void* buffer, size_t length, std::shared_ptr<Address> from, int flags) {
    m_remoteAddress = from;
    socklen_t len = from->getAddrLen();
    return ::recvfrom(m_sock, buffer, length, flags, from->getAddr(), &len);
}

int Socket::recvFrom(iovec* buffers, size_t length, std::shared_ptr<Address> from, int flags) {
    m_remoteAddress = from;
    msghdr msg;
    memset(&msg, 0, sizeof(msg));
    msg.msg_iov = (iovec*)buffers;
    msg.msg_iovlen = length;
    msg.msg_name = from->getAddr();
    msg.msg_namelen = from->getAddrLen();
    return ::recvmsg(m_sock, &msg, flags);
}

std::ostream& Socket::dump(std::ostream& os) const {
    os << "[Socket sock=" << m_sock
        << " is_connected=" << m_isConnected
        << " family=" << m_family
        << " type=" << m_type
        << " protocol=" << m_protocol;
    if(m_localAddress) {
        os << " local_address=" << m_localAddress->toString();
    }
    if(m_remoteAddress) {
        os << " remote_address=" << m_remoteAddress->toString();
    }
    os << "]";
    return os;
}

std::string Socket::toString() const {
    std::stringstream ss;
    dump(ss);
    return ss.str();
}

std::shared_ptr<Address> Socket::getRemoteAddress() {
    if(m_remoteAddress) {
        return m_remoteAddress;
    }
    std::shared_ptr<Address> result;
    switch(m_family) {
        case AF_INET:
            result.reset(new IPv4Address());
            break;
        case AF_INET6:
            result.reset(new IPv6Address());
            break;
        case AF_UNIX:
            result.reset(new UnixAddress());
            break;
        default:
            result.reset(new UnknownAddress(m_family));
            break;
    }
    socklen_t addrlen = result->getAddrLen();
    if (getpeername(m_sock, result->getAddr(), &addrlen)) {//只有connect后的UDP sockfd 或者 TCP sockfd可以获得对端地址,如果是Unix sockfd 也可以获得地址
        return std::make_shared<UnknownAddress>(m_family);
    }
    if (m_family == AF_UNIX) {
        std::shared_ptr<UnixAddress> addr = std::dynamic_pointer_cast<UnixAddress>(result);
        addr->setAddrLen(addrlen);
    }
    m_remoteAddress = result;
    return m_remoteAddress;
}

std::shared_ptr<Address> Socket::getLocalAddress() {
    if(m_localAddress) {
        return m_localAddress;
    }
    std::shared_ptr<Address> result;
    switch(m_family) {
        case AF_INET:
            result.reset(new IPv4Address());
            break;
        case AF_INET6:
            result.reset(new IPv6Address());
            break;
        case AF_UNIX:
            result.reset(new UnixAddress());
            break;
        default:
            result.reset(new UnknownAddress(m_family));
            break;
    }
    socklen_t addrlen = result->getAddrLen();
    if(getsockname(m_sock, result->getAddr(), &addrlen)) {//只有connect后的UDP sockfd 或者 TCP sockfd可以获得对端地址,如果是Unix sockfd 也可以获得地址
        YXTWebCpp_LOG_ERROR(g_logger) << "getsockname error sock=" << m_sock
            << " errno=" << errno << " errstr=" << strerror(errno);
        return std::make_shared<UnknownAddress>(m_family);
    }
    if(m_family == AF_UNIX) {
        std::shared_ptr<UnixAddress> addr = std::dynamic_pointer_cast<UnixAddress>(result);
        addr->setAddrLen(addrlen);
    }
    m_localAddress = result;
    return m_localAddress;
}

int Socket::getError() {
    int error = 0;
    socklen_t len = sizeof(error);
    if (!getOption(SOL_SOCKET, SO_ERROR, error)) {
        error = errno;
    }
    return error;
}

bool Socket::cancelRead() {
    return IOManager::GetThis()->cancelEvent(m_sock, IOManager::READ);
}

bool Socket::cancelWrite() {
    return IOManager::GetThis()->cancelEvent(m_sock, IOManager::WRITE);
}

bool Socket::cancelAccept() {
    return IOManager::GetThis()->cancelEvent(m_sock, IOManager::READ);
}

bool Socket::cancelAll() {
    return IOManager::GetThis()->cancelAll(m_sock);
}

void Socket::initSock() {
    int val = 1;
    setOption(SOL_SOCKET, SO_REUSEADDR, val);
    if (m_type == TCP) {
        setOption(IPPROTO_TCP,TCP_NODELAY, val);//关闭nagle算法
    }
}

void Socket::newSock() {
    m_sock = socket(m_family, m_type, m_protocol);
    if (m_sock != -1) {
        initSock();
    } else {
        YXTWebCpp_LOG_ERROR(g_logger) << "socket(" << m_family
            << ", " << m_type << ", " << m_protocol << ") errno="
            << errno << " errstr=" << strerror(errno);
    }
}

bool Socket::init(int sock) {
    //if (is_hook_enable()) {
        
        auto fdContext = FdMgr::GetInstance()->get(sock);//将该sockfd加入FdManager的管辖
        if (!(fdContext && fdContext->isSocket() && !fdContext->isClose())) {
            YXTWebCpp_LOG_DEBUG(g_logger) << "set NONBlock";
            return false;
        }
    //}
    m_sock = sock;
    m_isConnected = true;
    initSock();
    getLocalAddress();//由于是已经connect的TCP sockfd， 可以获得两端的address
    getRemoteAddress();
    return true;
}

std::ostream& operator<<(std::ostream& os, const Socket& sock) {
    os << sock.toString();
    return os;
}

}
