/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-10-03 11:09:02
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-10-03 12:07:32
 * @FilePath: /yxtweb-cpp/tests/testutil.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "../yxtwebcpp/yxtwebcpp.hpp"

void fun3() {
    YXTWebCpp_ASSERT(false);
}

void fun2() {
    fun3();
}

void fun1() {
    fun2();
}

int main() {
    YAML::Node node = YAML::LoadFile("../tests/logs.yml");
    YXTWebCpp::Config::LoadFromYaml(node);
    fun1();
    return 0;
}

