/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-10-11 11:00:19
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-10-12 21:50:40
 * @FilePath: /yxtweb-cpp/yxtwebcpp/hook.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "hook.hpp"
#include <dlfcn.h>
#include <stdarg.h>
#include "fd_manager.hpp"
#include "log.hpp"
#include "config.hpp"
#include "iomanager.hpp"

static std::shared_ptr<YXTWebCpp::Logger> g_logger = YXTWebCpp_LOG_NAME("system");
static std::shared_ptr<YXTWebCpp::ConfigVar<int> > g_tcp_connect_timeout = YXTWebCpp::Config::Lookup("tcp.connect.timeout", 5000, "tcp connect timeout");

namespace YXTWebCpp {

static thread_local bool t_hook_enable = false;

#define HOOK_FUN(XX) \
    XX(sleep) \
    XX(usleep) \
    XX(nanosleep)\
    XX(socket) \
    XX(connect) \
    XX(accept) \
    XX(read) \
    XX(readv) \
    XX(recv) \
    XX(recvfrom) \
    XX(recvmsg) \
    XX(write) \
    XX(writev) \
    XX(send) \
    XX(sendto) \
    XX(sendmsg) \
    XX(close) \
    XX(ioctl) \
    XX(setsockopt)

void hook_init() {
    static bool is_inited = false;
    if (is_inited) {
        return;
    }
//将自定义的函数指针都指向系统自带的函数
#define XX(name) name ## _f = (name ## _fun)dlsym(RTLD_NEXT, #name);
    HOOK_FUN(XX);
#undef XX
    is_inited = true;
}

static uint64_t s_connect_timeout = -1;

struct HookIniter {
    HookIniter() {
        hook_init();
        s_connect_timeout = g_tcp_connect_timeout->getValue();
        g_tcp_connect_timeout->addListener([](const int& old_value, const int& new_value) {
            s_connect_timeout = new_value;//如果通过setValue或者其它方式修改配置变量，那么s_connect_timeout会实时更改
        });//添加观察者
    }
};

static HookIniter s_hook_initer;

bool is_hook_enable() {
    return t_hook_enable;
}

void set_hook_enable(bool flag) {
    t_hook_enable = flag;
}

}

struct TimerInfo {
    int cancelled = 0;
};

template<typename OriginFun, typename ... Args>
static ssize_t do_io(int fd, OriginFun fun, const char* hook_fun_name, 
            uint32_t event, int timeout_so, Args&& ...args) {
    if (!YXTWebCpp::t_hook_enable) {//该线程没有挂hook
        return  fun(fd, std::forward<Args>(args)...);
    }
    std::shared_ptr<YXTWebCpp::FdContext> fdContext = YXTWebCpp::FdMgr::GetInstance()->get(fd);
    if (!fdContext) {//该fd没有被FdManager管辖
        return fun(fd, std::forward<Args>(args)...);
    }

    if (fdContext->isClose()) {//fd关闭了
        errno = EBADF;//bad file number
        return -1;
    }

    if (!fdContext->isSocket()) {//不是socket fd
        //一般不会进入
        return fun(fd, std::forward<Args>(args)...);
    }

    while (true) {
        uint64_t timeOut = fdContext->getTimeout(timeout_so);//获得该fd设置的超时时间
        std::shared_ptr<TimerInfo> sharedTimerInfo(new TimerInfo);
        
        size_t n = fun(fd, std::forward<Args>(args)...);
        while (n == -1 && errno == EINTR) {//io被中断
            n = fun(fd, std::forward<Args>(args)...);
        }
        YXTWebCpp::IOManager* iom = YXTWebCpp::IOManager::GetThis();
        std::shared_ptr<YXTWebCpp::Timer> timer;

        if (n == -1 && errno == EAGAIN) {//try again
            std::weak_ptr<TimerInfo> weakTimerInfo(sharedTimerInfo);
            if (timeOut != (uint64_t)-1) {//如果设置了fd的超时时间
                timer = iom->addConditionTimer(timeOut, [weakTimerInfo, fd, iom, event]() {//添加条件定时器
                    auto tmpSharedTimerInfo = weakTimerInfo.lock();
                    if (!tmpSharedTimerInfo || tmpSharedTimerInfo->cancelled) {
                        return;
                    }
                    tmpSharedTimerInfo->cancelled = ETIMEDOUT;//连接超时110
                    iom->cancelEvent(fd, (YXTWebCpp::IOManager::Event)(event));
                }, weakTimerInfo);//只要该定时器触发的时候，还没有退出while循环，那么这个定时器就有效
            }

            int rt = iom->addEvent(fd, (YXTWebCpp::IOManager::Event)(event));
            if (rt) {//添加事件失败
                YXTWebCpp_LOG_ERROR(g_logger) << hook_fun_name << "addEvent("
                    << fd << ", " << event ;
                if (timer) {
                    timer->cancel();//取消定时器
                }
            } else {//添加事件成功
                YXTWebCpp::Fiber::YieldToHold();
                if (timer) {
                    timer->cancel();
                }
                if (sharedTimerInfo->cancelled) {//如果sharedTimerInfo->cancelled有信息，那么说明超时定时器已经触发了
                    errno = sharedTimerInfo->cancelled;
                    return -1;
                }
            }
        } else {
            return n;
        }
    }
}

extern "C" {
//以下的宏用于定义自定义的函数指针
#define XX(name) name ## _fun name ## _f = nullptr;
    HOOK_FUN(XX);
#undef XX

unsigned int sleep(unsigned int seconds) {
    if (!YXTWebCpp::t_hook_enable) {
        return sleep_f(seconds);
    }
    std::shared_ptr<YXTWebCpp::Fiber> fiber = YXTWebCpp::Fiber::GetThis();//获得指向本协程的Fiber块
    YXTWebCpp::IOManager* iom = YXTWebCpp::IOManager::GetThis();
    iom->addTimer(seconds * 1000, [fiber, iom](){
        iom->schedule(fiber);
    });
    YXTWebCpp::Fiber::YieldToHold();
    return 0;
}

int usleep(useconds_t usec) {
    if (!YXTWebCpp::t_hook_enable) {
        return usleep_f(usec);
    }
    std::shared_ptr<YXTWebCpp::Fiber> fiber = YXTWebCpp::Fiber::GetThis();//获得指向本协程的Fiber块
    YXTWebCpp::IOManager* iom = YXTWebCpp::IOManager::GetThis();
    iom->addTimer(usec / 1000, [fiber, iom](){
        iom->schedule(fiber);
    });
    YXTWebCpp::Fiber::YieldToHold();
    return 0;
}

int nanosleep(const struct timespec *req, struct timespec *rem) {
    if (!YXTWebCpp::t_hook_enable) {
        return nanosleep_f(req, rem);
    }
    int timeout_ms = req->tv_sec * 1000 + req->tv_nsec / 1000 / 1000;
    std::shared_ptr<YXTWebCpp::Fiber> fiber = YXTWebCpp::Fiber::GetThis();//获得指向本协程的Fiber块
    YXTWebCpp::IOManager* iom = YXTWebCpp::IOManager::GetThis();
    iom->addTimer(timeout_ms, [fiber, iom](){
        iom->schedule(fiber);
    });
    YXTWebCpp::Fiber::YieldToHold();
    return 0;
}

int socket(int domain, int type, int protocol) {
    if (!YXTWebCpp::t_hook_enable) {
        return socket_f(domain, type, protocol);
    }

    int fd = socket_f(domain, type, protocol);
    if (fd >= 0) {
        YXTWebCpp::FdMgr::GetInstance()->get(fd, true);//加入FdManger的管辖范围
    }
    return fd;
}

int connect_with_timeout(int sockfd, const struct sockaddr *addr, socklen_t addrlen, uint64_t s_connect_timeout) {
    if (!YXTWebCpp::t_hook_enable) {
        return connect_f(sockfd, addr, addrlen);
    }
    auto context = YXTWebCpp::FdMgr::GetInstance()->get(sockfd);
    if (!context || !context->isClose()) {
        errno = EBADF;
        return -1;
    }
    if (!context->isSocket()) {
        return connect_f(sockfd, addr, addrlen);
    }

    if (context->getUserNonblock()) {
        return connect_f(sockfd, addr, addrlen);
    }

    int n = connect_f(sockfd, addr, addrlen);
    if (n == 0) {
        return 0;
    } else if (n != -1 || errno != EINPROGRESS) {
        return n;
    }

    YXTWebCpp::IOManager* iom = YXTWebCpp::IOManager::GetThis();
    std::shared_ptr<YXTWebCpp::Timer> timer;
    std::shared_ptr<TimerInfo> sharedTimerInfo(new TimerInfo);
    std::weak_ptr<TimerInfo> weakTimerInfo(sharedTimerInfo);

    if(s_connect_timeout != (uint64_t)-1) {
        timer = iom->addConditionTimer(s_connect_timeout, [weakTimerInfo, sockfd, iom]() {
                auto tmpSharedTimerInfo = weakTimerInfo.lock();
                if(!tmpSharedTimerInfo || tmpSharedTimerInfo->cancelled) {
                    return;
                }
                tmpSharedTimerInfo->cancelled = ETIMEDOUT;
                iom->cancelEvent(sockfd, YXTWebCpp::IOManager::WRITE);
        }, weakTimerInfo);
    }

    int rt = iom->addEvent(sockfd, YXTWebCpp::IOManager::WRITE);
    if(rt == 0) {
        YXTWebCpp::Fiber::YieldToHold();
        if(timer) {
            timer->cancel();
        }
        if(sharedTimerInfo->cancelled) {//说明已经触发了条件超时定时器
            errno = sharedTimerInfo->cancelled;
            return -1;
        }
    } else {
        if(timer) {
            timer->cancel();
        }
        YXTWebCpp_LOG_ERROR(g_logger) << "connect addEvent(" << sockfd << ", WRITE) error";
    }

    int error = 0;
    socklen_t len = sizeof(int);
    if(-1 == getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len)) {
        return -1;
    }
    if(!error) {
        return 0;
    } else {
        errno = error;
        return -1;
    }
}

int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    return connect_with_timeout(sockfd, addr, addrlen, YXTWebCpp::s_connect_timeout);
}

int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
    int fd = do_io(sockfd, accept_f, "accept", YXTWebCpp::IOManager::READ, SO_RCVTIMEO, addr, addrlen);
    if (fd >= 0) {//返回了一个新的socket fd
        YXTWebCpp::FdMgr::GetInstance()->get(fd, true);//把它纳入FdManager的管辖
    }
    return fd;
}

ssize_t read(int fd, void *buf, size_t count) {
    return do_io(fd, read_f, "read", YXTWebCpp::IOManager::READ, SO_RCVTIMEO, buf, count);
}

ssize_t readv(int fd, const struct iovec *iov, int iovcnt) {
    return do_io(fd, readv_f, "readv", YXTWebCpp::IOManager::READ, SO_RCVTIMEO, iov, iovcnt);
}

ssize_t recv(int sockfd, void *buf, size_t len, int flags) {
    return do_io(sockfd, recv_f, "recv", YXTWebCpp::IOManager::READ, SO_RCVTIMEO, buf, len, flags);
}

ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen) {
    return do_io(sockfd, recvfrom_f, "recvfrom", YXTWebCpp::IOManager::READ, SO_RCVTIMEO, buf, len, flags, src_addr, addrlen);
}

ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags) {
    return do_io(sockfd, recvmsg_f, "recvmsg", YXTWebCpp::IOManager::READ, SO_RCVTIMEO, msg, flags);
}

ssize_t write(int fd, const void *buf, size_t count) {
    return do_io(fd, write_f, "write", YXTWebCpp::IOManager::WRITE, SO_SNDTIMEO, buf, count);
}

ssize_t writev(int fd, const struct iovec *iov, int iovcnt) {
    return do_io(fd, writev_f, "writev", YXTWebCpp::IOManager::WRITE, SO_SNDTIMEO, iov, iovcnt);
}

ssize_t send(int sockfd, const void *buf, size_t len, int flags) {
    return do_io(sockfd, send_f, "send", YXTWebCpp::IOManager::WRITE, SO_SNDTIMEO, buf, len, flags);
}

ssize_t sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen) {
    return do_io(sockfd, sendto_f, "sendto", YXTWebCpp::IOManager::WRITE, SO_SNDTIMEO, buf, len, flags, dest_addr, addrlen);
}

ssize_t sendmsg(int sockfd, const struct msghdr *msg, int flags) {
    return do_io(sockfd, sendmsg_f, "sendmsg", YXTWebCpp::IOManager::WRITE, SO_SNDTIMEO, msg, flags);
}

int close(int fd) {
    if (!YXTWebCpp::t_hook_enable) {
        return close_f(fd);
    }

    auto context = YXTWebCpp::FdMgr::GetInstance()->get(fd);
    if (context) {
        auto iom = YXTWebCpp::IOManager::GetThis();
        if (iom) {
            iom->cancelAll(fd);//取消关于该fd的所有监听事件
        }
        YXTWebCpp::FdMgr::GetInstance()->del(fd);
    }
    return close_f(fd);
}

int ioctl(int fd, unsigned long request, ...) {
    va_list va;
    va_start(va, request);
    void* arg = va_arg(va, void*);
    va_end(va);

    if (FIONBIO == request) {
        bool userNonblock = (*(int*)arg != 0);
        auto context = YXTWebCpp::FdMgr::GetInstance()->get(fd);
        if (!context || context->isClose() || !context->isSocket()) {
            return ioctl_f(fd, request, arg);
        }
        context->setUserNonblock(userNonblock);
    }
    return ioctl_f(fd, request, arg);
}

int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen) {
    if (!YXTWebCpp::t_hook_enable) {
        return setsockopt_f(sockfd, level, optname, optval, optlen);
    }
    if (level = SOL_SOCKET) {
        if (optname == SO_RCVTIMEO || optname == SO_SNDTIMEO) {
            auto context = YXTWebCpp::FdMgr::GetInstance()->get(sockfd);
            if (context) {
                const timeval* v = (const timeval*)optval;
                context->setTimeout(optname, v->tv_sec * 1000 + v->tv_usec / 1000);
            }
        }
    }
    return setsockopt_f(sockfd, level, optname, optval, optlen);
}

}
