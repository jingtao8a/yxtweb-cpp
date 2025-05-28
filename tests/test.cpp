/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-09-17 16:37:43
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-10-19 12:39:26
 * @FilePath: /YXTWebCpp/tests/test.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "../yxtwebcpp/yxtwebcpp.hpp"
#include <iomanip>
#include <map>
#include <unordered_map>


class Base {
public:
    virtual void fun1() {
        std::cout << "Base::fun1" << std::endl;
    }
};

class Son : public Base {
public:
    virtual void fun1() {
        std::cout << "Son::sonFun" << std::endl;
    }
};

struct Node{ 
    int m_a;
    int m_b;
    Node(int a, int b):m_a(a), m_b(b) {}
    struct compartor {
        bool operator()(const std::shared_ptr<Node>& lhs, const std::shared_ptr<Node>& rhs) {
            if (lhs->m_a < rhs->m_a) {
                return true;
            } else if (lhs->m_a > rhs->m_a) {
                return false;
            } else {
                if (lhs->m_b < rhs->m_b) {
                    return true;
                } else if (lhs->m_b > rhs->m_b) {
                    return false;
                } else {
                    return lhs.get() < rhs.get();
                }
            }
        }
    };
};

static uint32_t EncodeZigzag32(const int32_t& v) {
        return ((uint32_t)v << 1) ^ (v >> 31);
}

static int32_t DecodeZigzag32(const uint32_t& v) {
    return (v >> 1) ^ -(v & 1);
}

static uint64_t EncodeZigzag64(const int64_t& v) {
        return ((uint64_t)v << 1) ^ (v >> 63);
}

static int64_t DecodeZigzag64(const uint64_t& v) {
    return (v >> 1) ^ -(v & 1);
}


class He : public std::enable_shared_from_this<He> {
public:
    He(int age, std::string name):m_age(age), m_name(name) {}

    void fun() {
        std::cout << m_age << " " << m_name << std::endl;
    }

    operator bool() {
        return true;
    }
private:
    int m_age;
    std::string m_name;
};

int main() {
    std::map<std::string, int> hash;
    hash.insert({"12", 12});
    hash.insert({"13", 13});
    auto res = hash.erase("11");
    std::cout << res << std::endl;
    for (auto& p : hash) {
        std::cout << p.first << "=" << p.second << std::endl;
    }
    return 0;
}