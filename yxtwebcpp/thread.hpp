/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-09-29 11:13:57
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-09-29 19:44:35
 * @FilePath: /yxtweb-cpp/yxtwebcpp/thread.hpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef _THREAD_HPP_
#define _THREAD_HPP_

#include <thread>
#include <functional>
#include <memory>
#include <string>
#include <pthread.h>
#include "nocopyable.hpp"
#include "mutex.hpp"


namespace YXTWebCpp {

class Thread : public Nocopyable{
public:
    Thread(std::function<void()> cb, const std::string& name);
    ~Thread();

    const std::string getName() const { return mThreadName; }
    const pid_t getThreadId() const { return mRealThreadId; }//获取线程的唯一标识
    void join();

public:
    static Thread* GetThis();
    static const std::string GetName();
    static void SetName(const std::string& name);
    
private:
    static void* run(void *);

private:
    pid_t mRealThreadId = -1;//线程唯一标识ID
    pthread_t mThreadId = 0;//线程ID
    std::string mThreadName;//线程名
    std::function<void()> m_cb;
    Semaphore m_semaphore;
};

}

#endif