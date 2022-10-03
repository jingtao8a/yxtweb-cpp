/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-09-29 16:59:43
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-09-29 19:27:29
 * @FilePath: /yxtweb-cpp/tests/test_thread.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "../yxtwebcpp/yxtwebcpp.hpp"

void fun1() {
    for (int i = 0; i < 100; ++i) {
        sleep(1);
        std::cout << YXTWebCpp::Thread::GetName() << std::endl;
    }
}

void fun2() {
    for (int i = 0; i < 100; ++i) {
        sleep(1);
        std::cout << YXTWebCpp::Thread::GetName() << std::endl;
    }
}

int main() {
    YXTWebCpp::Thread thread1(fun1, "fun1");
    YXTWebCpp::Thread thread2(fun2, "fun2");
    while (1) {}
    return 0;
}