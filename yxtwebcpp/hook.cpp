/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-10-11 11:00:19
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-10-11 14:07:29
 * @FilePath: /yxtweb-cpp/yxtwebcpp/hook.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "hook.hpp"
#include "iomanager.hpp"
#include <dlfcn.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
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
    XX(write)

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

struct HookIniter {
    HookIniter() {
        hook_init();
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

}
