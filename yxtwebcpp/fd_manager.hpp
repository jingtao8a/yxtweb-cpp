/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-09-29 10:27:26
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-09-29 20:05:48
 * @FilePath: /yxtweb-cpp/yxtwebcpp/fd_manager.hpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef _FD_MANAGER_HPP_
#define _FD_MANAGER_HPP_

#include <vector>
#include "mutex.hpp"
#include "singleton.hpp"

namespace YXTWebCpp{

class FdContext: std::enable_shared_from_this<FdContext> {
public:
    FdContext(int fd);

    ~FdContext();
    
    bool isInit() const { return m_isInit;}

    bool isSocket() const { return m_isSocket;}

    bool isClose() const { return m_isClosed;}

    void setUserNonblock(bool v) { m_userNonblock = v;}

    bool getUserNonblock() const { return m_userNonblock;}

    void setSysNonblock(bool v) { m_sysNonblock = v;}

    bool getSysNonblock() const { return m_sysNonblock;}

    void setTimeout(int type, uint64_t v);

    uint64_t getTimeout(int type);

private:
    bool init();

private:
    bool m_isInit: 1;
    bool m_isSocket: 1;
    bool m_sysNonblock: 1;
    bool m_userNonblock: 1;
    bool m_isClosed : 1;
    int m_fd;//文件句柄
    uint64_t m_recvTimeout;
    uint64_t m_sendTimeout;
};

class FdManager {
public:
    FdManager();

    std::shared_ptr<FdContext> get(int fd, bool auto_create = false);

    void del(int fd);
private:
    Mutex m_mutex;
    std::vector<std::shared_ptr<FdContext> > m_datas;
};

typedef Singleton<FdManager> FdMgr;

}
#endif