/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-09-29 15:24:45
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-10-17 10:57:25
 * @FilePath: /yxtweb-cpp/yxtwebcpp/util.hpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef _UTIL_HPP_
#define _UTIL_HPP_

#include <pthread.h>
#include <stdint.h>
#include <vector>
#include <string>

namespace YXTWebCpp {

pid_t GetThreadId();

uint32_t GetFiberId();

void Backtrace(std::vector<std::string>& vec, size_t size = 64, size_t skip = 2);

std::string  BacktraceToString(size_t size = 64, size_t skip = 2, const std::string& prefix = "");

//获得当前时间
uint64_t GetCurrentMS();//毫秒

uint64_t GetCurrentUS();//微秒

std::string Time2Str(time_t ts, const std::string& format);

time_t Str2Time(const char* str, const char* format);

class StringUtil {
public:
    static std::string Format(const char* fmt, ...);
    static std::string Formatv(const char* fmt, va_list ap);

    static std::string UrlEncode(const std::string& str, bool space_as_plus = true);
    static std::string UrlDecode(const std::string& str, bool space_as_plus = true);

    static std::string Trim(const std::string& str, const std::string& delimit = " \t\r\n");
    static std::string TrimLeft(const std::string& str, const std::string& delimit = " \t\r\n");
    static std::string TrimRight(const std::string& str, const std::string& delimit = " \t\r\n");


    static std::string WStringToString(const std::wstring& ws);
    static std::wstring StringToWString(const std::string& s);

};

}

#endif