/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-10-11 11:00:13
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-10-12 20:36:29
 * @FilePath: /yxtweb-cpp/yxtwebcpp/hook.hpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef _HOOK_HPP
#define _HOOK_HPP
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <fcntl.h>
#include <sys/ioctl.h>

namespace YXTWebCpp {

bool is_hook_enable();

void set_hook_enable(bool flag);

}

extern "C" {//C没有函数重载
//sleep
    typedef unsigned int (*sleep_fun)(unsigned int seconds);
    extern sleep_fun sleep_f;

    typedef int (*usleep_fun)(useconds_t usec);
    extern usleep_fun usleep_f;

    typedef int (*nanosleep_fun)(const struct timespec *req, struct timespec *rem);
    extern nanosleep_fun nanosleep_f;

    // unsigned int sleep(unsigned int seconds);
    // int usleep(useconds_t usec);
    // int nanosleep(const struct timespec *req, struct timespec *rem);
    
//socket
    typedef int (*socket_fun)(int domain, int type, int protocol);
    extern socket_fun socket_f;

    typedef int (*connect_fun)(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
    extern connect_fun connect_f;

    typedef int (*accept_fun)(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
    extern accept_fun accept_f;

    //int socket(int domain, int type, int protocol);
    //int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
    //int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);

//read
    typedef ssize_t (*read_fun)(int fd, void *buf, size_t count);
    extern read_fun read_f;

    typedef ssize_t (*readv_fun)(int fd, const struct iovec *iov, int iovcnt);
    extern readv_fun readv_f;

    typedef ssize_t (*recv_fun)(int sockfd, void *buf, size_t len, int flags);
    extern recv_fun recv_f;

    typedef ssize_t (*recvfrom_fun)(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen);
    extern recvfrom_fun recvfrom_f;

    typedef ssize_t (*recvmsg_fun)(int sockfd, struct msghdr *msg, int flags);
    extern recvmsg_fun recvmsg_f;

    
    
    //ssize_t read(int fd, void *buf, size_t count);
    //ssize_t readv(int fd, const struct iovec *iov, int iovcnt);
    // ssize_t recv(int sockfd, void *buf, size_t len, int flags);
    //ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen);
    //ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags);

//write
    typedef ssize_t (*write_fun)(int fd, const void *buf, size_t count);
    extern write_fun write_f;

    typedef ssize_t (*writev_fun)(int fd, const struct iovec *iov, int iovcnt);
    extern writev_fun writev_f;

    typedef ssize_t (*send_fun)(int sockfd, const void *buf, size_t len, int flags);
    extern send_fun send_f;

    typedef ssize_t (*sendto_fun)(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen);
    extern sendto_fun sendto_f;

    typedef ssize_t (*sendmsg_fun)(int sockfd, const struct msghdr *msg, int flags);
    extern sendmsg_fun sendmsg_f;

    //ssize_t write(int fd, const void *buf, size_t count);
    // ssize_t writev(int fd, const struct iovec *iov, int iovcnt);
    // ssize_t send(int sockfd, const void *buf, size_t len, int flags);
    // ssize_t sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen);
    // ssize_t sendmsg(int sockfd, const struct msghdr *msg, int flags);


    typedef int (*close_fun)(int fd);
    extern close_fun close_f;
    // int close(int fd);

    // typedef int (*fcntl_fun)(int fd, int cmd, ... /* arg */ );
    // extern fcntl_fun fcntl_f;
    // int fcntl(int fd, int cmd, ... /* arg */ );

    typedef int (*ioctl_fun)(int fd, unsigned long request, ...);
    extern ioctl_fun ioctl_f;
    // int ioctl(int fd, unsigned long request, ...);

    // typedef int (*getsockopt_fun)(int sockfd, int level, int optname, void *optval, socklen_t *optlen);
    // extern getsockopt_fun getsockopt_f;
    typedef int (*setsockopt_fun)(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
    extern setsockopt_fun setsockopt_f;
    // int getsockopt(int sockfd, int level, int optname, void *optval, socklen_t *optlen);
    // int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
}

#endif