/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-10-17 13:52:27
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-10-17 13:52:37
 * @FilePath: /yxtweb-cpp/yxtwebcpp/http/http11_common.hpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef _http11_common_h
#define _http11_common_h

#include <sys/types.h>

typedef void (*element_cb)(void *data, const char *at, size_t length);
typedef void (*field_cb)(void *data, const char *field, size_t flen, const char *value, size_t vlen);

#endif