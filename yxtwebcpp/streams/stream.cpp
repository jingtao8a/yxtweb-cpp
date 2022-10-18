/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-10-18 18:42:47
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-10-18 18:50:52
 * @FilePath: /yxtweb-cpp/yxtwebcpp/stream.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "stream.hpp"

namespace YXTWebCpp {

int Stream::readFixSize(void* buffer, size_t length) {
    size_t offset = 0;
    int64_t left = length;//缓冲区剩余大小
    while(left > 0) {
        int64_t len = read((char*)buffer + offset, left);
        if(len <= 0) {//len为已经读取出byte，如果为0代表对端close，为-1代表读取出错
            return len;
        }
        offset += len;
        left -= len;
    }
    return length;
}

int Stream::readFixSize(std::shared_ptr<ByteArray> ba, size_t length) {
    int64_t left = length;
    while(left > 0) {
        int64_t len = read(ba, left);
        if(len <= 0) {
            return len;
        }
        left -= len;
    }
    return length;
}

int Stream::writeFixSize(const void* buffer, size_t length) {
    size_t offset = 0;
    int64_t left = length;//还未发送完的数据byte
    while(left > 0) {
        int64_t len = write((const char*)buffer + offset, left);
        if(len <= 0) {
            return len;
        }
        offset += len;
        left -= len;
    }
    return length;

}

int Stream::writeFixSize(std::shared_ptr<ByteArray> ba, size_t length) {
    int64_t left = length;
    while(left > 0) {
        int64_t len = write(ba, left);
        if(len <= 0) {
            return len;
        }
        left -= len;
    }
    return length;
}

}

