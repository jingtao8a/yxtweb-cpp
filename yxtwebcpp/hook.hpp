/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-10-11 11:00:13
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-10-11 14:06:12
 * @FilePath: /yxtweb-cpp/yxtwebcpp/hook.hpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef _HOOK_HPP
#define _HOOK_HPP

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

    typedef ssize_t (*read_fun)(int fd, void *buf, size_t count);
    extern read_fun read_f;

    typedef ssize_t (*write_fun)(int fd, const void *buf, size_t count);
    extern write_fun write_f;
    //int socket(int domain, int type, int protocol);
    //int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
    //int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
    //ssize_t read(int fd, void *buf, size_t count);
    //ssize_t write(int fd, const void *buf, size_t count);
}

#endif