/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-10-06 11:40:57
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-10-14 14:32:59
 * @FilePath: /yxtweb-cpp/yxtwebcpp/endian.hpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef _ENDIAN_HPP_
#define _ENDIAN_HPP_
#define YXTWebCpp_LITTLE_ENDIAN 1
#define YXTWebCpp_BIG_ENDIAN 2

#include <byteswap.h>
#include <stdint.h>
#include <type_traits>

namespace YXTWebCpp {

template<class T>
typename std::enable_if< sizeof(T) == sizeof(uint64_t), T>::type
byteswap(T value) {
    return (T)bswap_64((uint64_t)value);
}

template<class T>
typename std::enable_if< sizeof(T) == sizeof(uint32_t), T>::type
byteswap(T value) {
    return (T)bswap_32((uint32_t)value);
}

template<class T>
typename std::enable_if< sizeof(T) == sizeof(uint16_t), T>::type
byteswap(T value) {
    return (T)bswap_16((uint16_t)value);
}

#if BYTE_ORDER == BIG_ENDIAN 
#define YXTWebCpp_BYTE_ORDER YXTWebCpp_BIG_ENDIAN
#else
#define YXTWebCpp_BYTE_ORDER YXTWebCpp_LITTLE_ENDIAN
#endif

#if YXTWebCpp_BYTE_ORDER == YXTWebCpp_BIG_ENDIAN

template<class T>
T byteswapOnLittleEndian(T value) {
    return value;
}

template<class T>
T byteswapOnBigEndian(T value) {
    return byteswap(value);
}

#else

template<class T>
T byteswapOnLittleEndian(T value) {
    return byteswap(value);
}

template<class T>
T byteswapOnBigEndian(T value) {
    return value;
}

#endif

}

#endif