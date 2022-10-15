/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-10-14 10:49:39
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-10-15 14:46:45
 * @FilePath: /yxtweb-cpp/yxtwebcpp/address.hpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */

#ifndef _ADDRESS_HPP_
#define _ADDRESS_HPP_
#include <stdint.h>
#include <memory>
#include <sys/types.h>
#include <sys/socket.h>
#include <ifaddrs.h>
#include <sys/un.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <vector>
#include <map>

namespace YXTWebCpp {

class IPAddress;

class Address {
public:
    static std::shared_ptr<Address> Create(const sockaddr* addr, __socklen_t addrlen);

    static bool Lookup(std::vector<std::shared_ptr<Address> >& result, const std::string& host,
            int family = AF_INET, int type = 0, int protocol = 0);//查找域名或主机号的所有Address

    static std::shared_ptr<Address> LookupAny(const std::string& host,
            int family = AF_INET, int type = 0, int protocol = 0);//查找域名或主机号对应的任意一个Address

    static std::shared_ptr<IPAddress> LookupAnyIPAddress(const std::string& host,
            int family = AF_INET, int type = 0, int protocol = 0);//查找域名或主机号对应的任意一个IPAddress

    static bool GetInterfaceAddresses(std::multimap<std::string
                    ,std::pair<std::shared_ptr<Address>, uint32_t> >& result,
                    int family = AF_INET);//返回本机所有网卡<网卡名，地址，子网掩码数>

    static bool GetInterfaceAddresses(std::vector<std::pair<std::shared_ptr<Address>, uint32_t> >&result
                    ,const std::string& iface, int family = AF_INET);//获取指定网卡的地址和掩码数

public:
    virtual ~Address() {}
    virtual const sockaddr* getAddr() const = 0;//返回sockaddr指针，只读
    virtual sockaddr* getAddr() = 0;//返回sockaddr指针，可读写
    virtual socklen_t getAddrLen() const = 0;//返回sockaddr的长度
    virtual std::ostream& insert(std::ostream& os) const = 0;

public:
    int getFamily() const;//返回协议簇
    std::string toString() const;
    bool operator<(const Address& rhs) const;
    bool operator==(const Address& rhs) const;
    bool operator!=(const Address& rhs) const;
};

class IPAddress : public Address {
public:
    //address可以为域名或者IP
    static std::shared_ptr<IPAddress> Create(const char* address, uint16_t port = 0);

public:
    //prefix_len为子网掩码位数
    virtual std::shared_ptr<IPAddress>  broadcastAddress(uint32_t prefix_len) = 0;
    //返回网络号
    virtual std::shared_ptr<IPAddress> networkAddress(uint32_t prefix_len) = 0;
    //返回子网掩码
    virtual std::shared_ptr<IPAddress> subnetMask(uint32_t prefix_len) = 0;
    //返回端口号
    virtual uint16_t getPort() const = 0;
    //设置端口号
    virtual void setPort(uint16_t v) = 0;
};

class IPv4Address : public IPAddress {
public:
    //address为IP字符串
    static std::shared_ptr<IPv4Address> Create(const char *address, uint16_t port = 0);

public:
    IPv4Address();
    IPv4Address(const sockaddr_in& address);
    IPv4Address(uint32_t address, uint16_t port = 0);

public:
    const sockaddr* getAddr() const override;
    sockaddr* getAddr() override;
    socklen_t getAddrLen() const override;
    std::ostream& insert(std::ostream& os) const override;

    std::shared_ptr<IPAddress> broadcastAddress(uint32_t prefix_len) override;
    std::shared_ptr<IPAddress> networkAddress(uint32_t prefix_len) override;
    std::shared_ptr<IPAddress> subnetMask(uint32_t prefix_len) override;
    uint16_t getPort() const override;
    void setPort(uint16_t v) override;

private:
    sockaddr_in m_addr;
};

class IPv6Address : public IPAddress {
public:
    //address为IP字符串
    static std::shared_ptr<IPv6Address> Create(const char* address, uint16_t port = 0);
public:
    IPv6Address();
    IPv6Address(const sockaddr_in6& address);
    IPv6Address(const uint8_t address[16], uint16_t port = 0);

public:
    const sockaddr* getAddr() const override;
    sockaddr* getAddr() override;
    socklen_t getAddrLen() const override;
    std::ostream& insert(std::ostream& os) const override;

    std::shared_ptr<IPAddress> broadcastAddress(uint32_t prefix_len) override;
    std::shared_ptr<IPAddress> networkAddress(uint32_t prefix_len) override;
    std::shared_ptr<IPAddress> subnetMask(uint32_t prefix_len) override;
    uint16_t getPort() const override;
    void setPort(uint16_t v) override;

private:
    sockaddr_in6 m_addr;
};

class UnixAddress : public Address {
public:
    UnixAddress();
    UnixAddress(const std::string& path);

public:
    void setAddrLen(uint32_t v);
    std::string getPath() const;

public:
    const sockaddr* getAddr() const override;
    sockaddr* getAddr() override;
    socklen_t getAddrLen() const override;
    std::ostream& insert(std::ostream& os) const override;

private:
    sockaddr_un m_addr;
    socklen_t m_length;
};

class UnknownAddress : public Address{
public:
    UnknownAddress() {}
    UnknownAddress(int family);
    UnknownAddress(const sockaddr& addr);
public:
    const sockaddr* getAddr() const override;
    sockaddr* getAddr() override;
    socklen_t getAddrLen() const override;
    std::ostream& insert(std::ostream& os) const override;
private:
    sockaddr m_addr;
};

std::ostream& operator<<(std::ostream& os, const Address& addr);

}


#endif