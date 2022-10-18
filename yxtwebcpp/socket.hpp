/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-10-15 11:03:57
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-10-18 11:04:34
 * @FilePath: /yxtweb-cpp/yxtwebcpp/socket.hpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef _SOCKET_HPP_
#define _SOCKET_HPP_
#include <memory>
#include <sys/types.h>
#include <sys/socket.h>
#include "nocopyable.hpp"
#include "address.hpp"

namespace YXTWebCpp {

class Socket : public std::enable_shared_from_this<Socket>, Nocopyable {
public:
    enum Family {
        IPv4 = AF_INET,
        IPv6 = AF_INET6,
        UNIX = AF_UNIX
    };

    enum Type {
        TCP = SOCK_STREAM,
        UDP = SOCK_DGRAM
    };
    
public:
    //创建TCP Socket(满足地址类型)
    static std::shared_ptr<Socket> CreateTCP(std::shared_ptr<Address> address);
    //创建UDP Socket(满足地址类型)
    static std::shared_ptr<Socket> CreateUDP(std::shared_ptr<Address> address);
    //创建一个IPv4的TCP Socket对象,未创建sock fd
    static std::shared_ptr<Socket> CreateTCPSocket();
    //创建一个IPv4的UDP Socket对象,已创建sock fd
    static std::shared_ptr<Socket> CreateUDPSocket();
    //创建一个IPv6的TCP Socket对象,未创建sock fd
    static std::shared_ptr<Socket> CreateTCPSocket6();
    //创建一个IPv6的UDP Socket对象,已创建sock fd
    static std::shared_ptr<Socket> CreateUDPSocket6();
    //创建一个Unix的TCP Socket对象,未创建sock fd
    static std::shared_ptr<Socket> CreateUnixTCPSocket();
    //创建一个Unix的UDP Socket对象,未创建sock fd
    static std::shared_ptr<Socket> CreateUnixUDPSocket();

public:
    Socket(int family, int type, int protocol = 0);

    virtual ~Socket();//close(m_sock)

    //获取和设置超时时间
    uint64_t getSendTimeout();
    void setSendTimeout(uint64_t v);
    uint64_t getRecvTimeout();
    void setRecvTimeout(uint64_t v);

    bool getOption(int level, int option, void* result, socklen_t* len);
    template<typename T>
    bool getOption(int level, int option, T& result) {
        socklen_t length = sizeof(T);
        return getOption(level, option, &result, &length);
    }

    bool setOption(int level, int option, const void* result, socklen_t len);
    template<typename T>
    bool setOption(int level, int option, const T& value) {
        return setOption(level, option, &value, sizeof(T));
    }

public:
    virtual bool bind(const std::shared_ptr<Address> addr);
    virtual bool listen(int backlog = SOMAXCONN);
    virtual std::shared_ptr<Socket> accept();
    virtual bool close();
    virtual bool connect(const std::shared_ptr<Address> addr, uint64_t timeout_ms = -1);
    virtual bool reconnect(uint64_t timeout_ms = -1);
    
    virtual int send(const void* buffer, size_t length, int flags = 0);
    virtual int send(const iovec*buffers, size_t length, int flags = 0);
    virtual int sendTo(const void* buffer, size_t length, const std::shared_ptr<Address> to, int flags = 0);
    virtual int sendTo(const iovec* buffers, size_t length, const std::shared_ptr<Address> to, int flags = 0);

    virtual int recv(void *buffer, size_t length, int flags = 0);
    virtual int recv(iovec* buffers, size_t length, int flags = 0);
    virtual int recvFrom(void* buffer, size_t length, std::shared_ptr<Address> from, int flags = 0);
    virtual int recvFrom(iovec* buffers, size_t length, std::shared_ptr<Address> from, int flags = 0);

    virtual std::ostream& dump(std::ostream& os) const;
    virtual std::string toString() const;

public:
    std::shared_ptr<Address> getRemoteAddress();
    std::shared_ptr<Address> getLocalAddress();
    
    int getSocket() const { return m_sock; }
    int getFamily() const { return m_family; }
    int getType() const { return m_type; }
    int getProtocol() const { return m_type; }
    bool isConnected() const { return m_isConnected; }
    bool isValid() const { return m_sock != -1; }
    int getError();
    bool cancelRead();
    bool cancelWrite();
    bool cancelAccept();
    bool cancelAll();

protected:
    void initSock();//设置sock未SO_RESUEADDR，如果是TCP的sock 关闭Nagle算法
    void newSock();
    virtual bool init(int sock);

protected:
    int m_sock = -1;
    int m_family;
    int m_type;
    int m_protocol;
    bool m_isConnected;
    std::shared_ptr<Address> m_localAddress;//本地地址
    std::shared_ptr<Address> m_remoteAddress;//远端地址
};

}

#endif