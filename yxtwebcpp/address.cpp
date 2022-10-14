/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-10-14 10:49:24
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-10-14 20:29:58
 * @FilePath: /yxtweb-cpp/yxtwebcpp/address.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "address.hpp"
#include "log.hpp"
#include "endian.hpp"
#include "macro.hpp"
#include <string>
#include <sys/types.h>

static std::shared_ptr<YXTWebCpp::Logger> g_logger = YXTWebCpp_LOG_NAME("system");

namespace YXTWebCpp {

template<typename T>
static T CreateMask(uint32_t bits) {
    return (1 << (sizeof(T) * 8 - bits)) - 1;
}

template<typename T>
static uint32_t CountNumOfOne(T value) {//统计value中1的个数
    uint32_t result = 0;
    while (value) {
        value &= (value - 1);
        ++result;
    }
    return result;
}

//Address
//静态
std::shared_ptr<Address> Address::Create(const sockaddr* addr, __socklen_t addrlen) {
    if (addr == nullptr) {
        return nullptr;
    }
    switch(addr->sa_family) {
        case AF_INET:
            return std::make_shared<IPv4Address>(*(const sockaddr_in*)addr);
        case AF_INET6:
            return std::make_shared<IPv6Address>(*(const sockaddr_in6*)addr);
        default:
            return std::make_shared<UnknownAddress>(*addr);
    }
    YXTWebCpp_ASSERT2(false, "never reach");
}

bool Address::Lookup(std::vector<std::shared_ptr<Address> >& result, const std::string& host,
            int family, int type, int protocol) {//查找域名的所有Address
    std::string node;
    const char* service = NULL;

    if (!host.empty() && host[0] == '[') {//可能是一个IPv6地址字符串
        const char* endipv6 = (const char*)memchr(host.c_str() + 1, ']', host.size() - 1);
        if (endipv6) {//sure
            if (*(endipv6 + 1) == ':') {
                service = endipv6 + 2;//指向端口字符串，或者服务名称
            }
            node = host.substr(1, endipv6 - host.c_str() + 1);//存储IPv6地址
        }
    }

    if (node.empty()) {//可能是一个IPv4地址字符串
        service = (const char*)memchr(host.c_str(), ':', host.size());
        if (service) {
            if (!memchr(service + 1, ':', host.c_str() + host.size() - service - 1)) {//sure
                node = host.substr(0, service - host.c_str());//存储IPv4地址
            }
                ++service;//指向端口字符串,或者服务名称
        }
    }

    if (node.empty()) {//可能是一个域名
        node = host;
    }
    addrinfo hints, *results, *next;
    hints.ai_flags = 0;
    hints.ai_family = family;
    hints.ai_socktype = type;
    hints.ai_protocol = protocol;
    hints.ai_addrlen = 0;
    hints.ai_canonname = NULL;//规范名称
    hints.ai_addr = NULL;
    hints.ai_next = NULL;
    int error = getaddrinfo(node.c_str(), service, &hints, &results);
    if(error) {
        YXTWebCpp_LOG_DEBUG(g_logger) << "Address::Lookup getaddress(" << host << ", "
            << family << ", " << type << ") err=" << error << " errstr="
            << gai_strerror(error);
        return false;
    }

    next = results;
    while (next) {
        result.push_back(Create(next->ai_addr, next->ai_addrlen));
        next = next->ai_next;
    }
    freeaddrinfo(results);
    return !result.empty();
}

std::shared_ptr<Address> Address::LookupAny(const std::string& host,
            int family, int type, int protocol) {//查找域名对应的任意一个Address
    std::vector<std::shared_ptr<Address> > result;
    if (Lookup(result, host, family, type, protocol)) {
        return result[0];
    }
    return nullptr;    
}

std::shared_ptr<IPAddress> Address::LookupAnyIPAddress(const std::string& host,
            int family, int type, int protocol) {//查找域名对应的任意一个IPAddress
    std::vector<std::shared_ptr<Address> > result;
    if (Lookup(result, host, family, type, protocol)) {
        for (auto& i : result) {
            std::shared_ptr<IPAddress> v = std::dynamic_pointer_cast<IPAddress>(i);
            if (v) {
                return v;
            }
        }
    }
    return nullptr; 
}

bool Address::GetInterfaceAddresses(std::multimap<std::string, std::pair<std::shared_ptr<Address>, uint32_t> >& result,
                    int family) {//返回本机所有网卡<网卡名，地址， 子网掩码数>
    ifaddrs *next = nullptr, *results = nullptr;
    if (getifaddrs(&results) != 0) {
        YXTWebCpp_LOG_DEBUG(g_logger) << "Address::GetInterfaceAddresses getifaddrs "
            " err=" << errno << " errstr=" << strerror(errno);
        return false;
    }

    try {
        next = results;
        while (next) {
            std::shared_ptr<Address> addr;
            uint32_t prefix_len = ~0u;
            if (family != AF_UNSPEC && family != next->ifa_addr->sa_family) {
                next = next->ifa_next;
                continue;
            }

            
            switch (next->ifa_addr->sa_family) {
                case AF_INET:
                    {
                        addr = Create(next->ifa_addr, sizeof(next->ifa_addr));
                        uint32_t netmask = ((sockaddr_in*)next->ifa_netmask)->sin_addr.s_addr;
                        prefix_len = CountNumOfOne(netmask);
                    }
                    break;
                case AF_INET6:
                    {
                        in6_addr& netmask = ((sockaddr_in6*)next->ifa_netmask)->sin6_addr;
                        for (int i = 0; i < 16; ++i) {
                            prefix_len += CountNumOfOne(netmask.s6_addr[i]);
                        }
                    }
                    break;
            } 
            if (addr) {
                result.insert(std::make_pair(next->ifa_name, std::make_pair(addr, prefix_len)));
            }
            next = next->ifa_next;
        }
    } catch(...) {
        YXTWebCpp_LOG_ERROR(g_logger) << "Address::GetInterfaceAddresses exception";
        freeifaddrs(results);
        return false;
    }
    freeifaddrs(results);
    return !result.empty();
}

bool Address::GetInterfaceAddresses(std::vector<std::pair<std::shared_ptr<Address>, uint32_t> >&result
                    ,const std::string& iface, int family) {//获取指定网卡的地址和掩码数
    if (iface.empty() || iface == "*") {
        return false;
    }
    std::multimap<std::string, std::pair<std::shared_ptr<Address>, uint32_t> > results;
    if (!GetInterfaceAddresses(results, family)) {
        return false;
    }
    auto its = results.equal_range(iface);
    while (its.first != its.second) {
        auto tmp = *(its.first);
        result.push_back(tmp.second);
        ++its.first;
    }
    return !result.empty();
}

//非静态
//模板方法模式
int Address::getFamily() const {//返回协议簇
    return getAddr()->sa_family;
}

std::string Address::toString() const {
    std::stringstream ss;
    insert(ss);
    return ss.str();
}

bool Address::operator<(const Address& rhs) const {
    socklen_t minlen = std::min(getAddrLen(), rhs.getAddrLen());
    int result = memcmp(getAddr(), rhs.getAddr(), minlen);
    if (result < 0) {
        return true;
    } else if (result > 0) {
        return false;
    } else if (getAddrLen() < rhs.getAddrLen()){
        return true;
    } else {
        return false;
    }
}

bool Address::operator==(const Address& rhs) const {
    return getAddrLen() == rhs.getAddrLen() && memcmp(getAddr(), rhs.getAddr(), getAddrLen()) == 0;
}

bool Address::operator!=(const Address& rhs) const {
    return !(*this == rhs);
}

//IPAddress
//静态
std::shared_ptr<IPAddress> IPAddress::Create(const char* address, uint16_t port) {
    addrinfo hints, *results;
    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_UNSPEC;
    
    int error = getaddrinfo(address, NULL, &hints, &results);
    if (error) {
        YXTWebCpp_LOG_DEBUG(g_logger) << "IPAddress::Create(" << address
            << ", " << port << ") error=" << error
            << " errno=" << errno << " errstr=" << strerror(errno);
        return nullptr;
    }
    try {
        auto result = std::dynamic_pointer_cast<IPAddress>(Address::Create(results->ai_addr, (socklen_t)results->ai_addrlen));
        if (result) {
            result->setPort(port);
        }
        freeaddrinfo(results);
        return result;
    } catch(...) {
        freeaddrinfo(results);
        return nullptr;
    }
}

//IPv4Address
//静态
std::shared_ptr<IPv4Address> IPv4Address::Create(const char *address, uint16_t port) {
    auto result = std::make_shared<IPv4Address>();
    result->m_addr.sin_port = byteswapOnLittleEndian(port);
    result->m_addr.sin_addr.s_addr = inet_addr(address);
    return result;
}

//非静态
IPv4Address::IPv4Address() {
    memset(&m_addr, 0, sizeof(m_addr));
    m_addr.sin_family = AF_INET;
}

IPv4Address::IPv4Address(const sockaddr_in& address) {
    m_addr = address;
}

IPv4Address::IPv4Address(uint32_t address, uint16_t port) {
    memset(&m_addr, 0, sizeof(m_addr));
    m_addr.sin_family = AF_INET;
    m_addr.sin_addr.s_addr = byteswapOnLittleEndian(address);
    m_addr.sin_port = byteswapOnLittleEndian(port);
}
//Address interface
const sockaddr* IPv4Address::getAddr() const {
    return (sockaddr*)&m_addr;
}

sockaddr* IPv4Address::getAddr() {
    return (sockaddr*)&m_addr;
}

socklen_t IPv4Address::getAddrLen() const {
    return sizeof(m_addr);
}

std::ostream& IPv4Address::insert(std::ostream& os) const {
    uint32_t addr = byteswapOnLittleEndian(m_addr.sin_addr.s_addr);
    os  << ((addr >> 24) & 0xff) << "."
        << ((addr >> 16) & 0xff) << "."
        << ((addr >> 8) & 0xff) << "."
        << (addr & 0xff);
    os << ":" << byteswapOnLittleEndian(m_addr.sin_port);
    return os;
}

//IPAddress interface
std::shared_ptr<IPAddress> IPv4Address::broadcastAddress(uint32_t prefix_len) {
    sockaddr_in baddr(m_addr);
    baddr.sin_addr.s_addr |= byteswapOnLittleEndian(CreateMask<uint32_t>(prefix_len));
    return std::make_shared<IPv4Address>(baddr);
}

std::shared_ptr<IPAddress> IPv4Address::networkAddress(uint32_t prefix_len) {
    auto subnetptr = std::dynamic_pointer_cast<IPv4Address>(subnetMask(prefix_len));
    if (!subnetptr) {
        return nullptr;
    }
    sockaddr_in baddr(m_addr);
    baddr.sin_addr.s_addr &= subnetptr->m_addr.sin_addr.s_addr; 
    return std::make_shared<IPv4Address>(baddr);
}

std::shared_ptr<IPAddress> IPv4Address::subnetMask(uint32_t prefix_len) {
    uint32_t mask = CreateMask<uint32_t>(prefix_len);
    sockaddr_in subnet;
    subnet.sin_family = AF_INET;
    subnet.sin_addr.s_addr = byteswapOnLittleEndian(~mask);
    return std::make_shared<IPv4Address>(subnet);
}

uint16_t IPv4Address::getPort() const {
    return byteswapOnLittleEndian(m_addr.sin_port);
}

void IPv4Address::setPort(uint16_t v) {
    m_addr.sin_port = byteswapOnLittleEndian(v);
}

//IPv6Address
//静态
std::shared_ptr<IPv6Address> IPv6Address::Create(const char* address, uint16_t port) {
    auto result = std::make_shared<IPv6Address>();
    result->m_addr.sin6_port = byteswapOnLittleEndian(port);
    inet_pton(AF_INET6, address, &result->m_addr.sin6_addr.s6_addr);
    return result;
}

//非静态
IPv6Address::IPv6Address() {
    memset(&m_addr, 0, sizeof(m_addr));
    m_addr.sin6_family = AF_INET6;
}

IPv6Address::IPv6Address(const sockaddr_in6& address) {
    m_addr = address;
}

IPv6Address::IPv6Address(const uint8_t address[16], uint16_t port) {
    memset(&m_addr, 0, sizeof(m_addr));
    m_addr.sin6_family = AF_INET6;
    m_addr.sin6_port = byteswapOnLittleEndian(port);
    memcpy(&m_addr.sin6_addr.s6_addr, address, 16);
}

//Address interface
const sockaddr* IPv6Address::getAddr() const {
    return (sockaddr*)&m_addr;
}
sockaddr* IPv6Address::getAddr() {
    return (sockaddr*)&m_addr;
}

socklen_t IPv6Address::getAddrLen() const {
    return sizeof(m_addr);
}

std::ostream& IPv6Address::insert(std::ostream& os) const {
    os << "[";
    bool use_zeros = false;
    uint16_t* addr = (uint16_t*)m_addr.sin6_addr.s6_addr;
    for (size_t i = 0; i < 8; ++i) {
        if (addr[i] == 0 && !use_zeros) {
            continue;
        }
        if (i && addr[i - 1] == 0 && !use_zeros) {
            use_zeros = true;
            os << ":";
        }
        if (i) {
            os << ":";
        }
        os << std::hex << byteswapOnLittleEndian(addr[i]) << std::dec;
    }
    os << "]:" << byteswapOnLittleEndian(m_addr.sin6_port);
    return os;
}

//IPAddress interface
std::shared_ptr<IPAddress> IPv6Address::broadcastAddress(uint32_t prefix_len) {
    sockaddr_in6 baddr(m_addr);
    baddr.sin6_addr.s6_addr[prefix_len / 8] |= CreateMask<uint8_t>(prefix_len % 8);
    for (size_t i = prefix_len / 8; i < 16; ++i) {
        baddr.sin6_addr.s6_addr[i] = 0xff;
    }
    return std::make_shared<IPv6Address>(baddr);
}

std::shared_ptr<IPAddress> IPv6Address::networkAddress(uint32_t prefix_len) {
    sockaddr_in6 baddr(m_addr);
    baddr.sin6_addr.s6_addr[prefix_len / 8] &= (~CreateMask<uint8_t>(prefix_len % 8));
    for (size_t i = prefix_len / 8; i < 16; ++i) {
        baddr.sin6_addr.s6_addr[i] = 0x00;
    }
    return std::make_shared<IPv6Address>(baddr);
}

std::shared_ptr<IPAddress> IPv6Address::subnetMask(uint32_t prefix_len) {
    sockaddr_in6 subnet;
    memset(&subnet, 0 ,sizeof(subnet));
    subnet.sin6_family = AF_INET6;
    subnet.sin6_addr.s6_addr[prefix_len / 8] = (~CreateMask<uint8_t>(prefix_len % 8));
    for (size_t i = 0; i < prefix_len / 8; ++i) {
        subnet.sin6_addr.s6_addr[i] = 0xff;
    }
    return std::make_shared<IPv6Address>(subnet);
}

uint16_t IPv6Address::getPort() const{
    return byteswapOnLittleEndian(m_addr.sin6_port);
}
void IPv6Address::setPort(uint16_t v) {
    m_addr.sin6_port = byteswapOnLittleEndian(v);
}

static const size_t MAX_PATH_LEN = sizeof(((sockaddr_un*)0)->sun_path) - 1;
//UnixAddress
//非静态
UnixAddress::UnixAddress() {
    memset(&m_addr, 0, sizeof(m_addr));
    m_addr.sun_family = AF_UNIX;
    m_length = offsetof(sockaddr_un, sun_path) + MAX_PATH_LEN;//除去了放置"\0"的长度
}

UnixAddress::UnixAddress(const std::string& path) {
    memset(&m_addr, 0, sizeof(m_addr));
    m_addr.sun_family = AF_UNIX;
    m_length = path.size() + 1;
    if (!path.empty() && path[0] == '\0') {//path首位为'\0'
        --m_length;
    }
    if (m_length > sizeof(m_addr.sun_path)) {
        throw std::logic_error("path too run");
    }
    memcpy(m_addr.sun_path, path.c_str(), m_length);
    m_length += offsetof(sockaddr_un, sun_path);
}

void UnixAddress::setAddrLen(uint32_t v) {
    m_length = v;
}

std::string UnixAddress::getPath() const {
    std::stringstream ss;
    if (m_length > offsetof(sockaddr_un, sun_path) && m_addr.sun_path[0] == '\0') {
        ss << "\\0" << std::string(m_addr.sun_path + 1, m_length - offsetof(sockaddr_un, sun_path) - 1);
    } else {
        ss << m_addr.sun_path;
    }
    return ss.str();
}

//Address interface
const sockaddr* UnixAddress::getAddr() const {
    return (sockaddr*)&m_addr;
}

sockaddr* UnixAddress::getAddr() {
    return (sockaddr*)&m_addr;
}

socklen_t UnixAddress::getAddrLen() const {
    return m_length;
}

std::ostream& UnixAddress::insert(std::ostream& os) const {
    if(m_length > offsetof(sockaddr_un, sun_path)
            && m_addr.sun_path[0] == '\0') {
        return os << "\\0" << std::string(m_addr.sun_path + 1, m_length - offsetof(sockaddr_un, sun_path) - 1);
    } else {
        return os << m_addr.sun_path;
    }
}

//UnknownAddress
//非静态
UnknownAddress::UnknownAddress(int family) {
    memset(&m_addr, 0, sizeof(m_addr));
    m_addr.sa_family = family;
}

UnknownAddress::UnknownAddress(const sockaddr& addr) {
    m_addr = addr;
}

//Address interface
const sockaddr* UnknownAddress::getAddr() const {
    return &m_addr;
}

sockaddr* UnknownAddress::getAddr() {
    return &m_addr;
}

socklen_t UnknownAddress::getAddrLen() const {
    return sizeof(m_addr);
}

std::ostream& UnknownAddress::insert(std::ostream& os) const {
    os << "[UnknownAddress family = " << m_addr.sa_family << "]";
    return os;
}


std::ostream& operator<<(std::ostream& os, const Address& addr) {
    return addr.insert(os);
}

}