/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-09-29 15:24:58
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-10-09 18:52:00
 * @FilePath: /yxtweb-cpp/yxtwebcpp/util.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "util.hpp"
#include <unistd.h>
#include <sys/syscall.h> 
#include <execinfo.h>
#include <sstream>
#include <sys/time.h>
#include "log.hpp"
#include "fiber.hpp"

namespace YXTWebCpp {

std::shared_ptr<Logger> g_logger = YXTWebCpp_LOG_NAME("system");

pid_t GetThreadId() {
    return syscall(SYS_gettid);//返回线程唯一标识ID
}

uint32_t GetFiberId() {
    return Fiber::GetFiberId();
}

void Backtrace(std::vector<std::string>& vec, size_t size, size_t skip) {
    void **buffer = (void **)malloc(sizeof(void *) * size);
    size_t s = backtrace(buffer, size);
    char** strings = backtrace_symbols(buffer, s);
    if (strings == nullptr) {
        YXTWebCpp_LOG_ERROR(g_logger) << "backtrace_symbols error";
    }
    for (int i = skip; i < s; ++i) {
        vec.push_back(strings[i]);
    }
    free(buffer);
    free(strings);
}

std::string  BacktraceToString(size_t size, size_t skip, const std::string& prefix) {
    std::vector<std::string> vec;
    Backtrace(vec, size, skip);
    std::stringstream ss;
    for (std::string& str : vec) {
        ss << prefix << str << std::endl;
    }
    return ss.str();
}


uint64_t GetCurrentMS() {//毫秒
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000uL + tv.tv_usec / 1000;
}

uint64_t GetCurrentUS() {//微秒
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 * 1000uL + tv.tv_usec;
}

}