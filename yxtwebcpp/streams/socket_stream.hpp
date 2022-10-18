/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-10-18 18:53:07
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-10-18 19:32:36
 * @FilePath: /yxtweb-cpp/yxtwebcpp/streams/socket_stream.hpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef _SOCKET_STREAM_HPP_
#define _SOCKET_STREAM_HPP_
#include "stream.hpp" 
#include "../socket.hpp"

namespace YXTWebCpp {

class SocketStream : public Stream {
public:
    SocketStream(std::shared_ptr<Socket> socket);
    ~SocketStream();

    int read(void* buffer, size_t length) override;
    int read(std::shared_ptr<ByteArray> ba, size_t length) override;
    int write(const void* buffer, size_t length) override;
    int write(std::shared_ptr<ByteArray> ba, size_t length) override;

    std::shared_ptr<Socket> getSocket() const { return m_socket; }
    void close();

    std::shared_ptr<Address>  getRemoteAddress();
    std::shared_ptr<Address> getLocalAddress();
    std::string getRemoteAddressString();
    std::string getLocalAddressString();
    
private:
    std::shared_ptr<Socket> m_socket;
};

}

#endif