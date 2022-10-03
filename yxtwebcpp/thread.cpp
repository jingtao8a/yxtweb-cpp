/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-09-29 11:14:05
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-10-02 19:40:41
 * @FilePath: /yxtweb-cpp/yxtwebcpp/thread.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "thread.hpp"
#include <stdexcept>
#include "log.hpp"
#include "util.hpp"

namespace YXTWebCpp {

static std::shared_ptr<Logger> g_logger = YXTWebCpp_LOG_NAME("system");

static thread_local Thread* t_thread = nullptr;
static thread_local std::string t_thread_name = "UNKNOWN";

Thread::Thread(std::function<void()> cb, const std::string& name): m_cb(cb), mThreadName(name){
    if (name.empty()) {
        mThreadName = "UNKNOWN";
    }
    int rt = pthread_create(&mThreadId, nullptr, Thread::run, this);
    if (rt) {
        YXTWebCpp_LOG_ERROR(g_logger) << "pthread_create failed, rt = " << rt
                << " m_thread_name = " << mThreadName;
        throw std::logic_error("pthread_create failed");
    }
    m_semaphore.wait();
}

Thread::~Thread() {
    if (mThreadId) {
        pthread_detach(mThreadId);
    }
}

void Thread::join() {
    if (mThreadId) {
        int rt = pthread_join(mThreadId, nullptr);
        if (rt) {
            YXTWebCpp_LOG_ERROR(g_logger) << "pthread_join failed, rt = " << rt
                    << " m_thread_name = " << mThreadName;
            throw std::logic_error("pthread_join error");
        }
        mThreadId = 0;
    }
}


//静态方法
void* Thread::run(void *arg) {
    Thread* thread = (Thread *)arg;
    t_thread = thread;
    t_thread_name = thread->mThreadName;
    thread->mRealThreadId = GetThreadId();
    pthread_setname_np(pthread_self(), thread->mThreadName.substr(0, 15).c_str());

    std::function<void()> cb;
    cb.swap(thread->m_cb);
    thread->m_semaphore.notify();
    cb();
    return 0;
}

//静态方法
Thread* Thread::GetThis() {
    return t_thread;
}

//静态方法
const std::string Thread::GetName() {
    return t_thread_name;
}

//静态方法
void Thread::SetName(const std::string& name) {
    if (t_thread) {
        t_thread->mThreadName = name;
    }
    t_thread_name = name;
}

}