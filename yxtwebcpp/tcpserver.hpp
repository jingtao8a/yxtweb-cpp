/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-10-18 11:01:00
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-10-18 20:08:12
 * @FilePath: /yxtweb-cpp/yxtwebcpp/tcpserver.hpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
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
protected:
    virtual void handleClient(std::shared_ptr<Socket> sock);
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