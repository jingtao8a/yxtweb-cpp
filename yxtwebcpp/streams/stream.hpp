/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-10-18 18:42:33
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-10-18 19:32:21
 * @FilePath: /yxtweb-cpp/yxtwebcpp/stream.hpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef _STREAM_HPP__
#define _STREAM_HPP__

#include <memory>
#include "../bytearray.hpp"

namespace YXTWebCpp {

class Stream {
public:
    virtual ~Stream() {}

    virtual int read(void* buffer, size_t length) = 0;

    virtual int read(std::shared_ptr<ByteArray> ba, size_t length) = 0;

    //FixSize表示必须要读取length长度的数据才可以退出
    virtual int readFixSize(void* buffer, size_t length);
    //FixSize表示必须要读取length长度的数据才可以退出
    virtual int readFixSize(std::shared_ptr<ByteArray> ba, size_t length);

    virtual int write(const void* buffer, size_t length) = 0;

    virtual int write(std::shared_ptr<ByteArray> ba, size_t length) = 0;

    virtual int writeFixSize(const void* buffer, size_t length);

    virtual int writeFixSize(std::shared_ptr<ByteArray> ba, size_t length);

    /**
     * @brief 关闭流
     */
    virtual void close() = 0;
};

}

#endif