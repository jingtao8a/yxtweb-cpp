/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-10-16 14:48:23
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-10-16 15:32:27
 * @FilePath: /yxtweb-cpp/tests/testbytearray.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "../yxtwebcpp/yxtwebcpp.hpp"

std::shared_ptr<YXTWebCpp::Logger> g_logger = YXTWebCpp_LOG_ROOT();

void test() {
    #define XX(type, len, write_fun, read_fun, base_len) {\
    std::vector<type> vec; \
    for(int i = 0; i < len; ++i) { \
        type tmp;\
        tmp = rand();\
        vec.push_back(tmp); \
    } \
    std::shared_ptr<YXTWebCpp::ByteArray> ba(new YXTWebCpp::ByteArray(base_len)); \
    for(auto& i : vec) { \
        ba->write_fun(i); \
    } \
    ba->setPosition(0); \
    for(size_t i = 0; i < vec.size(); ++i) { \
        type v = ba->read_fun(); \
        YXTWebCpp_ASSERT(v == vec[i]); \
    } \
    YXTWebCpp_ASSERT(ba->getReadSize() == 0); \
    YXTWebCpp_LOG_INFO(g_logger) << #write_fun "/" #read_fun \
                    " (" #type " ) len=" << len \
                    << " base_len=" << base_len \
                    << " size=" << ba->getSize(); \
}

    XX(int8_t,  100, writeFint8, readFint8, 100);
    XX(uint8_t, 100, writeFuint8, readFuint8, 100);
    XX(int16_t,  100, writeFint16,  readFint16, 1);
    XX(uint16_t, 100, writeFuint16, readFuint16, 1);
    XX(int32_t,  100, writeFint32,  readFint32, 1);
    XX(uint32_t, 100, writeFuint32, readFuint32, 1);
    XX(int64_t,  100, writeFint64,  readFint64, 1);
    XX(uint64_t, 100, writeFuint64, readFuint64, 1);

    XX(int32_t,  100, writeInt32,  readInt32, 1);
    XX(uint32_t, 100, writeUint32, readUint32, 1);
    XX(int64_t,  100, writeInt64,  readInt64, 1);
    XX(uint64_t, 100, writeUint64, readUint64, 1);
#undef XX

#define XX(type, len, write_fun, read_fun, base_len) {\
    std::vector<type> vec; \
    for(int i = 0; i < len; ++i) { \
        vec.push_back(rand()); \
    } \
    std::shared_ptr<YXTWebCpp::ByteArray> ba(new YXTWebCpp::ByteArray(base_len)); \
    for(auto& i : vec) { \
        ba->write_fun(i); \
    } \
    ba->setPosition(0); \
    for(size_t i = 0; i < vec.size(); ++i) { \
        type v = ba->read_fun(); \
        YXTWebCpp_ASSERT(v == vec[i]); \
    } \
    YXTWebCpp_ASSERT(ba->getReadSize() == 0); \
    YXTWebCpp_LOG_INFO(g_logger) << #write_fun "/" #read_fun \
                    " (" #type " ) len=" << len \
                    << " base_len=" << base_len \
                    << " size=" << ba->getSize(); \
    ba->setPosition(0); \
    YXTWebCpp_ASSERT(ba->writeToFile("./" #type "_" #len "-" #read_fun ".dat")); \
    std::shared_ptr<YXTWebCpp::ByteArray> ba2(new YXTWebCpp::ByteArray(base_len * 2)); \
    YXTWebCpp_ASSERT(ba2->readFromFile("./" #type "_" #len "-" #read_fun ".dat")); \
    ba2->setPosition(0); \
    YXTWebCpp_ASSERT(ba->toString() == ba2->toString()); \
    YXTWebCpp_ASSERT(ba->getPosition() == 0); \
    YXTWebCpp_ASSERT(ba2->getPosition() == 0); \
}
    XX(int8_t,  100, writeFint8, readFint8, 1);
    XX(uint8_t, 100, writeFuint8, readFuint8, 1);
    XX(int16_t,  100, writeFint16,  readFint16, 1);
    XX(uint16_t, 100, writeFuint16, readFuint16, 1);
    XX(int32_t,  100, writeFint32,  readFint32, 1);
    XX(uint32_t, 100, writeFuint32, readFuint32, 1);
    XX(int64_t,  100, writeFint64,  readFint64, 1);
    XX(uint64_t, 100, writeFuint64, readFuint64, 1);

    XX(int32_t,  100, writeInt32,  readInt32, 1);
    XX(uint32_t, 100, writeUint32, readUint32, 1);
    XX(int64_t,  100, writeInt64,  readInt64, 1);
    XX(uint64_t, 100, writeUint64, readUint64, 1);

#undef XX
}

int main() {
    test();
    return 0;
}