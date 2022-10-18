/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-10-18 18:53:15
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-10-18 19:28:02
 * @FilePath: /yxtweb-cpp/yxtwebcpp/streams/socket_stream.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "socket_stream.hpp"

namespace YXTWebCpp {

SocketStream::SocketStream(std::shared_ptr<Socket> socket):m_socket(socket) {}

SocketStream::~SocketStream() {}

int SocketStream::read(void* buffer, size_t length) {
    return m_socket->recv(buffer, length);
}

int SocketStream::read(std::shared_ptr<ByteArray> ba, size_t length) {
    std::vector<iovec> buffers;
    ba->getWriteBuffers(buffers, length);
    int len = m_socket->recv(&buffers[0], buffers.size());
    if (len > 0) {
        ba->setPosition(ba->getPosition() + len);//将ByteArray中m_position往后移动
    }
    return len;//返回已经读取的数据长度
}

int SocketStream::write(const void* buffer, size_t length) {
    return m_socket->send(buffer, length);
}

int SocketStream::write(std::shared_ptr<ByteArray> ba, size_t length) {
    std::vector<iovec> buffers;
    ba->getReadBuffers(buffers, length);
    int len = m_socket->send(&buffers[0], length);
    if (len > 0) {
        ba->setPosition(ba->getPosition() + len);//将m_position向后移动len个位置，len为已经发送的数据长度
    }
    return len;
}

void SocketStream::close() {
    if(m_socket) {
        m_socket->close();
    }
}

std::shared_ptr<Address> SocketStream::getRemoteAddress() {
    if(m_socket) {
        return m_socket->getRemoteAddress();
    }
    return nullptr;
}

std::shared_ptr<Address> SocketStream::getLocalAddress() {
    if(m_socket) {
        return m_socket->getLocalAddress();
    }
    return nullptr;
}

std::string SocketStream::getRemoteAddressString() {
    auto addr = getRemoteAddress();
    if(addr) {
        return addr->toString();
    }
    return "";
}

std::string SocketStream::getLocalAddressString() {
    auto addr = getLocalAddress();
    if(addr) {
        return addr->toString();
    }
    return "";
}

}