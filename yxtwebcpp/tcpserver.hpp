#ifndef _TCPSERVER_HPP_
#define _TCPSERVER_HPP_
#include "socket.hpp"
#include "iomanager.hpp"

namespace YXTWebCpp {

class TCPServer : public std::enable_shared_from_this<TCPServer> {
public:
    TCPServer(IOManager* worker = IOManager::GetThis(), IOManager* accept_worker = IOManager::GetThis());
    ~TCPServer();
    bool bind(std::shared_ptr<Address> addr);
    bool bind(std::vector<std::shared_ptr<Address> > addrs, std::vector<std::shared_ptr<Address> > failaddrs);
    void start();
    void stop();

    void setName(std::string name) { m_name = name; }
    std::string getName() const { return m_name; }

    void setRecvTimeout(uint64_t recvTimeout) { m_recvTimeout = recvTimeout; }
    uint64_t getRecvTimeout() const { return m_recvTimeout; }

private:
    void startAccept(std::shared_ptr<Socket> sock);
    void handleClient(std::shared_ptr<Socket> sock);
private:
    IOManager *m_worker;
    IOManager *m_accept_worker;
    std::vector<std::shared_ptr<Socket> > m_sockets;
    uint64_t m_recvTimeout;
    std::string m_name;//服务器名字
    bool isStop = true;
};

}


#endif