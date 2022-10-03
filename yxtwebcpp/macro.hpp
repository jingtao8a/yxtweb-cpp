/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-10-03 11:09:59
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-10-03 12:00:51
 * @FilePath: /yxtweb-cpp/yxtwebcpp/macro.hpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef _MACRO_HPP_
#define _MACRO_HPP_
#include "log.hpp"
#include "util.hpp"
#define YXTWebCpp_ASSERT(x) \
        if (!x) { \
            YXTWebCpp_LOG_ERROR(YXTWebCpp_LOG_ROOT()) << "ASSERTION: "#x \
            << "\nbacktrace:\n" \
            << YXTWebCpp::BacktraceToString(100, 2, "  ");\
            assert(x);\
        }

#define YXTWebCpp_ASSERT2(x, w) \
        if (!x) { \
            YXTWebCpp_LOG_ERROR(YXTWebCpp_LOG_ROOT()) << "ASSERTION: "#x \
            << "\n" << w\
            << "\nbacktrace:\n" \
            << YXTWebCpp::BacktraceToString(100, 2, "  ");\
            assert(x);\
        }
#endif