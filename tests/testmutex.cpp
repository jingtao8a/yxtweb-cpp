/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-09-29 20:42:03
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-10-03 10:48:17
 * @FilePath: /yxtweb-cpp/tests/testmutex.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "../yxtwebcpp/yxtwebcpp.hpp"
#include <vector>

int count = 0;
YXTWebCpp::RWMutex rwMutex;

void fun() {
    for (int i = 0; i < 1000000; ++i) {
        std::cout << YXTWebCpp::Thread::GetName() << std::endl;
        YXTWebCpp::ReadScopedLockImpl<YXTWebCpp::RWMutex> readlock(rwMutex);
        std::cout << count << std::endl;
    }
}

void fun2() {
    for (int i = 0; i < 10; ++i) {
        std::cout << YXTWebCpp::Thread::GetName() << std::endl;
        YXTWebCpp::WriteScopedLockImpl<YXTWebCpp::RWMutex> writelock(rwMutex);
        count++;
    }
}

int main() {
    YXTWebCpp::Thread t1(fun, "hello");
    t1.join();
    return 0;
}