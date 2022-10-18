/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-09-17 16:37:43
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-10-18 13:03:04
 * @FilePath: /YXTWebCpp/tests/test.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "../yxtwebcpp/yxtwebcpp.hpp"
#include <iomanip>
// static bool flag = true;

// static std::shared_ptr<YXTWebCpp::Logger> g_logger = YXTWebCpp_LOG_ROOT();

// void fun1() {
//     YXTWebCpp_LOG_DEBUG(g_logger) << "fun1";
// }


// void thread1() {
//     while (true) {
//         YXTWebCpp::Fiber::GetThis();
//         std::shared_ptr<YXTWebCpp::Fiber> p1(new YXTWebCpp::Fiber(fun1));
//         sleep(1);
//         p1->swapIn();
//         sleep(1);
//         p1->swapIn();
//     }
//     YXTWebCpp_LOG_DEBUG(g_logger) << "end";
// }


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
    // std::shared_ptr<YXTWebCpp::Logger> logger(new YXTWebCpp::Logger());
    // std::shared_ptr<YXTWebCpp::FileLogAppender> fileLogAppender(new YXTWebCpp::FileLogAppender("./logtxt.txt"));
    // logger->addAppender(fileLogAppender);
    // std::shared_ptr<YXTWebCpp::LogEvent> event = std::make_shared<YXTWebCpp::LogEvent>(logger, YXTWebCpp::LogLevel::Level::DEBUG, "file", 0, 0, 0, 0, 0, "thread");
    // event->format("haha%d", 100);
    // logger->log(event->getLevel(), event);
    
    // YXTWebCpp::Thread t(thread1, "test");
    // t.join();
    // Base* obj = new Son;
    // obj->Base::fun1();
    // std::set<int> rbtree{1, 23, 43};
    // auto it = rbtree.find(3);
    // rbtree.erase(it);
    // std::set<std::shared_ptr<Node>, Node::compartor> heap;
    // heap.insert(std::make_shared<Node>(1, 2));
    // heap.insert(std::make_shared<Node>(1, 5));
    // heap.insert(std::make_shared<Node>(3, 4));
    // auto ptr = std::make_shared<Node>(1, 4);

    // auto iter = heap.lower_bound(ptr);
    // std::cout << (*iter)->m_a << (*iter)->m_b;
    // std::cout << std::setw(6) << std::setfill('0') << std::hex << 0xABC << " ";
    // std::string str = "yuxintao";
    // std::cout << str.substr(5, -1);
    std::shared_ptr<He> he = std::make_shared<He>(21, "yuxintao");
    auto funobj = std::bind(&He::fun, he->shared_from_this());
    funobj();
    // printf("%p %p %p", &he, he, he.get());
    // std::shared_ptr<He> n;
    // std::cout << he.operator bool() << n.operator bool();
    //shared_ptr对象，用cout输出时，进行了重载，输出的是真实指针的地址
    //shared_ptr放在判断语句中，调用了operator bool重载，true为有指向对象，false为空
    return 0;
}