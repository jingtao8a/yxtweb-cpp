#ifndef _BYTEARRAY_HPP_
#define _BYTEARRAY_HPP_
#include <stddef.h>
#include <stdint.h>
#include <string>

namespace YXTWebCpp {

class ByteArray {
public:
    //指定大小的内存块
    struct Node {
        Node(size_t s);
        Node();
        ~Node();

        char* ptr;//内存块指针
        Node* next;//下一个Node
        size_t size;//内存块大小
    };

    ByteArray(size_t base_size = 4096);
    ~ByteArray();

    void writeFint8(int8_t value);
    void writeFuint8(uint8_t value);

    void writeFint16(int16_t value);
    void writeFuint16(uint16_t value);

    void writeFint32(int32_t value);
    void writeFuint32(uint32_t value);

    void writeFint64(int64_t value);
    void writeFuint64(uint64_t value);

    void writeInt32  (int32_t value);
    void writeUint32 (uint32_t value);

    void writeInt64  (int64_t value);
    void writeUint64 (uint64_t value);
    
    void writeFloat  (float value);
    void writeDouble (double value);

    /**
     * @brief 写入std::string类型的数据,用uint16_t作为长度类型
     * @post m_position += 2 + value.size()
     *       如果m_position > m_size 则 m_size = m_position
     */
    void writeStringF16(const std::string& value);

    /**
     * @brief 写入std::string类型的数据,用uint32_t作为长度类型
     * @post m_position += 4 + value.size()
     *       如果m_position > m_size 则 m_size = m_position
     */
    void writeStringF32(const std::string& value);

    /**
     * @brief 写入std::string类型的数据,用uint64_t作为长度类型
     * @post m_position += 8 + value.size()
     *       如果m_position > m_size 则 m_size = m_position
     */
    void writeStringF64(const std::string& value);

    /**
     * @brief 写入std::string类型的数据,用无符号Varint64作为长度类型
     * @post m_position += Varint64长度 + value.size()
     *       如果m_position > m_size 则 m_size = m_position
     */
    void writeStringVint(const std::string& value);

    /**
     * @brief 写入std::string类型的数据,无长度
     * @post m_position += value.size()
     *       如果m_position > m_size 则 m_size = m_position
     */
    void writeStringWithoutLength(const std::string& value);

    int8_t   readFint8();

    uint8_t  readFuint8();

    int16_t  readFint16();

    uint16_t readFuint16();

    int32_t  readFint32();

    uint32_t readFuint32();

    int64_t  readFint64();

    uint64_t readFuint64();

    int32_t  readInt32();

    uint32_t readUint32();

    int64_t  readInt64();

    uint64_t readUint64();

    float    readFloat();

    double   readDouble();

    std::string readStringF16();

    std::string readStringF32();

    std::string readStringF64();

    std::string readStringVint();

    void clear();

    void write(const void* buf, size_t size);

    void read(void* buf, size_t size);

    void read(void* buf, size_t size, size_t position) const;

    size_t getPosition() const { return m_position;}
    void setPosition(size_t v);

    bool writeToFile(const std::string& name) const;

    bool readFromFile(const std::string& name);

    size_t getBaseSize() const { return m_baseSize;}

    size_t getReadSize() const { return m_size - m_position;}

    bool isLittleEndian() const;

    void setIsLittleEndian(bool val);

    std::string toString() const;

    std::string toHexString() const;

    uint64_t getReadBuffers(std::vector<iovec>& buffers, uint64_t len = ~0ull) const;

    uint64_t getReadBuffers(std::vector<iovec>& buffers, uint64_t len, uint64_t position) const;

    uint64_t getWriteBuffers(std::vector<iovec>& buffers, uint64_t len);

    size_t getSize() const { return m_size;}
private:
    size_t m_capacity;
    size_t m_position;
    size_t m_baseSize;
    size_t m_size;
    int8_t m_endian;
    Node* m_root;
    Node* m_cur;
};

}

#endif