/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-10-08 13:33:06
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-10-09 12:59:31
 * @FilePath: /yxtweb-cpp/yxtwebcpp/iomanager.hpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef _IOMANAGER_HPP_
#define _IOMANAGER_HPP_
#include "scheduler.hpp"

namespace YXTWebCpp {

class IOManager : public Scheduler {
public:
    enum Event {
        NONE = 0x0,
        READ = 0x1,//EPOLLIN
        WRITE = 0X4//EPOLLOUT
    };

private:
    struct FdContext {
        struct EventContext {
            Scheduler* scheduler;//处理该事件的scheduler
            std::shared_ptr<Fiber> fiber; //处理该事件协程
            std::function<void()> cb; //处理该事件的回调函数
        };
        
        EventContext& getEventContext(Event event);//获得该event对应的EventContext
        void resetEventContext(EventContext& ctx);
        void triggerEvent(Event event);//触发该event对应的EventContext
        
        int fd;
        EventContext read;
        EventContext write;
        Event events = NONE;
        Mutex mutex;
    };

public:
    IOManager(size_t threads = 1, bool use_caller = true, const std::string& name = "");
    ~IOManager();

    //1 success, 0 retry, -1 error
    int addEvent(int fd, Event event, std::function<void()> cb = nullptr);
    bool delEvent(int fd, Event event);
    bool cancelEvent(int fd, Event event);
    bool cancelAll(int fd);

public:
    static IOManager* GetThis();

protected:
    void tickle() override;
    bool stopping() override;
    void idle() override;
    void contextResize(size_t size);

private:
    int m_epfd = 0;//epoll句柄
    int m_tickleFds[2];

    std::atomic<size_t> m_pendingEventCount = {0};//监听的事件数
    RWMutex m_mutex;//读写锁
    std::vector<FdContext *> m_fdContexts;//管理一组FdContext
};

}
#endif