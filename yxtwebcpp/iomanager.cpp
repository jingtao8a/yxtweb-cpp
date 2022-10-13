/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-10-08 13:33:14
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-10-13 20:13:13
 * @FilePath: /yxtweb-cpp/yxtwebcpp/iomanager.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "iomanager.hpp"
#include <sys/epoll.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include "macro.hpp"

namespace YXTWebCpp{

static std::shared_ptr<Logger> g_logger = YXTWebCpp_LOG_NAME("system");
IOManager::FdContext::EventContext& IOManager::FdContext::getEventContext(Event event) {
    switch (event) {
        case READ:
            return read;
        case WRITE:
            return write;
        default:
            YXTWebCpp_ASSERT2(false, "never reach");
    }
}

void IOManager::FdContext::resetEventContext(EventContext& context) {
    context.scheduler = nullptr;
    context.fiber.reset();
    context.cb = nullptr;
}

void IOManager::FdContext::triggerEvent(Event event) {
    YXTWebCpp_ASSERT(events & event);//触发的事件必须在该fd的events中包含了

    events = (Event)(events & ~event);//删除该事件
    EventContext& context = getEventContext(event);
    if (context.cb) {
        context.scheduler->schedule(&context.cb);
    } else {
        context.scheduler->schedule(&context.fiber);
    }
    resetEventContext(context);
}


IOManager::IOManager(size_t threads, bool use_caller, const std::string& name) 
    : Scheduler(threads, use_caller, name) {
    m_epfd = epoll_create(1);//创建epoll句柄
    YXTWebCpp_ASSERT(m_epfd > 0);

    int rt = pipe(m_tickleFds);//打开一对pipe套接字
    YXTWebCpp_ASSERT(rt == 0);

    rt = fcntl(m_tickleFds[0], F_SETFL, O_NONBLOCK);//将pipe的读端fd设置为非阻塞
    YXTWebCpp_ASSERT(rt != -1);

    epoll_event event;
    memset(&event, 0, sizeof(event));
    event.events = EPOLLIN | EPOLLET;//边缘触发
    event.data.fd = m_tickleFds[0];
    rt = epoll_ctl(m_epfd, EPOLL_CTL_ADD, m_tickleFds[0], &event);//将pipe的读端fd 边缘读事件加入epoll内核
    YXTWebCpp_ASSERT(rt == 0);

    contextResize(32);

    start();
}

IOManager::~IOManager() {
    stop();
    close(m_epfd);
    close(m_tickleFds[0]);
    close(m_tickleFds[1]);

    for (size_t i = 0; i < m_fdContexts.size(); ++i) {
        if (m_fdContexts[i]) {
            delete m_fdContexts[i];
        }
    }
}

//1 success, 0 retry, -1 error
int IOManager::addEvent(int fd, Event event, std::function<void()> cb) {
    FdContext* fd_context = nullptr;
    m_mutex.rdlock();
    if (m_fdContexts.size() > fd) {
        fd_context = m_fdContexts[fd];
        m_mutex.unlock();
    } else {
        m_mutex.unlock();
        WriteScopedLockImpl<RWMutex> guard(m_mutex);
        contextResize(fd * 1.5);
        fd_context = m_fdContexts[fd];
    }

    ScopedLockImpl<Mutex> guard(fd_context->mutex);
    YXTWebCpp_ASSERT(!(fd_context->events & event));//默认不给同一个fd添加重复事件

    int op = fd_context->events == NONE? EPOLL_CTL_ADD : EPOLL_CTL_MOD;//如果是fd原本无事件ADD,否则为MOD

    epoll_event epevent;
    epevent.events = EPOLLET | fd_context->events | event;
    epevent.data.ptr = fd_context;

    int rt = epoll_ctl(m_epfd, op, fd, &epevent);//往epoll内核ADD或MOD fd与事件
    YXTWebCpp_ASSERT(rt == 0);
    
    ++m_pendingEventCount;//监听的事件数加1
    fd_context->events = (Event)(fd_context->events | event);//fd句柄context的events更新
    FdContext::EventContext& eventContext = fd_context->getEventContext(event);//获得新增event对应的eventContext
    eventContext.scheduler = Scheduler::GetThis();//该线程对应的scheduler
    if (cb) {
        eventContext.cb.swap(cb);
    } else {
        //如果加事件的时候没有带function参数，将设置该事件的fiber为本协程的fiber
        eventContext.fiber = Fiber::GetThis();
        YXTWebCpp_ASSERT2(eventContext.fiber->getState() == Fiber::EXEC, "state = " << eventContext.fiber->getState());
    }
    return 0;
}

bool IOManager::delEvent(int fd, Event event) {
    m_mutex.rdlock();
    if ((int)m_fdContexts.size() <= fd) {
        m_mutex.unlock();
        return false;
    }
    FdContext* fdContext = m_fdContexts[fd];
    m_mutex.unlock();
    YXTWebCpp_ASSERT(fdContext->events & event);//被删除的事件要存在

    Event newEvents = (Event)(fdContext->events & ~event);
    int op = newEvents == NONE ? EPOLL_CTL_DEL : EPOLL_CTL_MOD;

    epoll_event epevent;
    epevent.events = EPOLLET | newEvents;
    epevent.data.ptr = fdContext;

    int rt = epoll_ctl(m_epfd, op, fd, &epevent);
    YXTWebCpp_ASSERT(!rt);

    --m_pendingEventCount;//监听事件数减1

    FdContext::EventContext& eventContext = fdContext->getEventContext(event);
    fdContext->resetEventContext(eventContext);
    fdContext->events = newEvents;
    return true;
}

bool IOManager::cancelEvent(int fd, Event event) {
    m_mutex.rdlock();
    if ((int)m_fdContexts.size() <= fd) {
        m_mutex.unlock();
        return false;
    }
    FdContext* fdContext = m_fdContexts[fd];
    m_mutex.unlock();
    YXTWebCpp_ASSERT(fdContext->events & event);//被取消的事件要存在

    Event newEvents = (Event)(fdContext->events & ~event);
    int op = newEvents == NONE ? EPOLL_CTL_DEL : EPOLL_CTL_MOD;

    epoll_event epevent;
    epevent.events = EPOLLET | newEvents;
    epevent.data.ptr = fdContext;
    
    int rt = epoll_ctl(m_epfd, op, fd, &epevent);
    YXTWebCpp_ASSERT(!rt);

    --m_pendingEventCount;
    fdContext->triggerEvent(event);
    return true;
}

bool IOManager::cancelAll(int fd) {
    m_mutex.rdlock();
    if (m_fdContexts.size() <= fd) {
        m_mutex.unlock();
        return false;
    }

    FdContext* fdContext = m_fdContexts[fd];
    m_mutex.unlock();

    ScopedLockImpl<Mutex> guard(fdContext->mutex);
    if (fdContext->events == NONE) {
        return false;
    }
    int op = EPOLL_CTL_DEL;
    epoll_event epevent;
    epevent.events = 0;
    epevent.data.ptr = fdContext;
    
    int rt = epoll_ctl(m_epfd, op, fd, &epevent);
    YXTWebCpp_ASSERT(!rt);
    if (fdContext->events & READ) {
        fdContext->triggerEvent(READ);
        --m_pendingEventCount;
    }

    if (fdContext->events & WRITE) {
        fdContext->triggerEvent(WRITE);
        --m_pendingEventCount;
    }
    return true;
}

void IOManager::tickle() {
    if (!hasIdleThreads()) {
        return;
    }
    int rt = write(m_tickleFds[1], "T", 1);//唤醒在epoll_wait的idle协程，表示有Fiber任务要处理了
    YXTWebCpp_ASSERT(rt == 1);
}

bool IOManager::stopping() {
    uint64_t time_out = 0;
    return stopping(time_out);
}

void IOManager::idle() {
    YXTWebCpp_LOG_DEBUG(g_logger) << "idle";
    const uint64_t MAX_EVENTS = 256;
    epoll_event* events = new epoll_event[MAX_EVENTS];
    std::shared_ptr<epoll_event> shared_events(events, [](epoll_event *ev) {
        delete[] ev;
    });//自定义删除器
    
    while (true) {
        uint64_t next_timeout = 0;
        if (stopping(next_timeout)) {
            break;
        }
        int rt = 0;
        do {
            static const int MAX_TIMEOUT = 1000;
            if (next_timeout != ~0uLL) {//有定时器的情况
                next_timeout = next_timeout < (uint64_t)MAX_TIMEOUT ? next_timeout : MAX_TIMEOUT;
            } else {//没有定时器的情况
                next_timeout = MAX_TIMEOUT;
            }
            rt = epoll_wait(m_epfd, events, MAX_EVENTS, MAX_TIMEOUT);//最多只阻塞等5sec
            if (rt < 0 && errno == EINTR) {
            } else {
                break;
            }
        } while (true);
        
        std::vector<std::function<void()> > cbs;
        listExpiredCb(cbs);
        if (!cbs.empty()) {
            // schedule<std::vector<std::function<void()> >::iterator >(cbs.begin(), cbs.end());
            for (auto cb : cbs) {
                schedule(cb);
            }
            cbs.clear();
        }

        for (int i = 0; i < rt; ++i) {
            epoll_event& event = events[i];
            if (event.data.fd == m_tickleFds[0]) {
                uint8_t dummy;
                while(read(m_tickleFds[0], &dummy, 1) == 1);
            } else {
                FdContext* fdContext = static_cast<FdContext *>(event.data.ptr);
                ScopedLockImpl<Mutex> guard(fdContext->mutex);
                //如果是对方主动关闭socket，会触发EPOLLRDHUP、EPOLLIN、EPOLLOUT事件
                if (event.events & (EPOLLERR | EPOLLHUP)) {//如果是自己方的socket出现问题，会触发该fd的EPOLLERR和EPOLLHUP事件
                    YXTWebCpp_LOG_DEBUG(g_logger) << "socket error";
                    event.events |= (EPOLLIN | EPOLLOUT) & fdContext->events;
                }
                int real_events = NONE;
                if (event.events & EPOLLIN) {
                    real_events |= EPOLLIN;
                }

                if (event.events & EPOLLOUT) {
                    real_events |= EPOLLOUT;
                }

                if (!(fdContext->events & real_events)) {
                    continue;
                }

                int left_events = (fdContext->events & ~real_events);
                int op = left_events == NONE ? EPOLL_CTL_DEL : EPOLL_CTL_MOD;

                event.events = left_events | EPOLLET;
                int rt2 = epoll_ctl(m_epfd, op, fdContext->fd, &event);
                YXTWebCpp_ASSERT(!rt2);
            
                if (real_events & READ) {
                    fdContext->triggerEvent(READ);
                    --m_pendingEventCount;
                }
                if (real_events & WRITE) {
                    fdContext->triggerEvent(WRITE);
                    --m_pendingEventCount;
                }
            }
        }
        Fiber::YieldToHold();
    }
}

void IOManager::onTimerInsertedAtFront() {
    tickle();
}

void IOManager::contextResize(size_t size) {
    m_fdContexts.resize(size);
    for (size_t i = 0; i < m_fdContexts.size(); ++i) {
        if (!m_fdContexts[i]) {
            m_fdContexts[i] = new FdContext;
            m_fdContexts[i]->fd = i;
        }
    }
}

bool IOManager::stopping(uint64_t& timeout) {
    timeout = getNextTimer();
    return timeout == ~0uLL
        && m_pendingEventCount == 0//监听事件数为0
        && Scheduler::stopping();
}


//静态成员函数
IOManager* IOManager::GetThis() {
    return dynamic_cast<IOManager*>(Scheduler::GetThis());
}


}


