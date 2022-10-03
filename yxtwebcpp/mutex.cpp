/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-09-29 10:27:26
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-09-29 19:55:15
 * @FilePath: /yxtweb-cpp/yxtwebcpp/mutex.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "mutex.hpp"

namespace YXTWebCpp {

Semaphore::Semaphore(uint32_t count) {//信号量初始值
    int rt = sem_init(&m_semaphore, 0, count);
    if (rt) {
        throw std::logic_error("sem_init failed");
    }
}

Semaphore::~Semaphore() {
    sem_destroy(&m_semaphore);
}

void Semaphore::wait() {
    sem_wait(&m_semaphore);
}
void Semaphore::notify() {
    sem_post(&m_semaphore);
}

}