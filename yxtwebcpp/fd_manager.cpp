/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-09-29 10:27:26
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-09-29 20:06:10
 * @FilePath: /yxtweb-cpp/yxtwebcpp/fd_manager.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "fd_manager.hpp"
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/socket.h>

namespace YXTWebCpp {

FdContext::FdContext(int fd)
    :m_isInit(false)
    ,m_isSocket(false)
    ,m_sysNonblock(false)
    ,m_userNonblock(false)
    ,m_isClosed(false)
    ,m_fd(fd)
    ,m_recvTimeout(-1)
    ,m_sendTimeout(-1) {
    init();
}

FdContext::~FdContext() {}

bool FdContext::init() {
    if(m_isInit) {
        return true;
    }
    m_recvTimeout = -1;
    m_sendTimeout = -1;

    struct stat fd_stat;
    if(-1 == fstat(m_fd, &fd_stat)) {
        m_isInit = false;
        m_isSocket = false;
    } else {
        m_isInit = true;
        m_isSocket = S_ISSOCK(fd_stat.st_mode);
    }

    if(m_isSocket) {//如果是socket fd
        int flags = fcntl(m_fd, F_GETFL, 0);
        if(!(flags & O_NONBLOCK)) {//设置为非阻塞状态
            fcntl(m_fd, F_SETFL, flags | O_NONBLOCK);
        }
        m_sysNonblock = true;
    } else {
        m_sysNonblock = false;
    }

    m_userNonblock = false;
    m_isClosed = false;
    return m_isInit;
}

void FdContext::setTimeout(int type, uint64_t v) {
    if(type == SO_RCVTIMEO) {
        m_recvTimeout = v;
    } else {
        m_sendTimeout = v;
    }
}


FdManager::FdManager() {
    m_datas.resize(64);
}

std::shared_ptr<FdContext> FdManager::get(int fd, bool auto_create) {
    if (fd == -1) {
        return nullptr;
    }
    {
        ScopedLockImpl<Mutex> guard(m_mutex);
        if ((int)m_datas.size() > fd && m_datas[fd]) {
            return m_datas[fd];
        }
    }
    if (!auto_create) {
        return nullptr;
    }
    ScopedLockImpl<Mutex> guard(m_mutex);
    std::shared_ptr<FdContext> context(new FdContext(fd));
    if ((int)m_datas.size() <= fd) {
        m_datas.resize(fd * 1.5);
    }
    m_datas[fd] = context;
    return context;
}

void FdManager::del(int fd) {
    ScopedLockImpl<Mutex> guard(m_mutex);
    if ((int)m_datas.size() <= fd) {
        return;
    }
    m_datas[fd].reset();
}

}