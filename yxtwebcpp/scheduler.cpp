/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-10-06 11:36:33
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-10-10 20:09:16
 * @FilePath: /yxtweb-cpp/yxtwebcpp/scheduler.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "scheduler.hpp"
#include "log.hpp"
#include "macro.hpp"
#include <iostream>

namespace YXTWebCpp {

static std::shared_ptr<Logger> g_logger = YXTWebCpp_LOG_NAME("system");

static thread_local Scheduler* t_scheduler = nullptr;
static thread_local Fiber* t_scheduler_fiber = nullptr;

Scheduler::Scheduler(size_t threads, bool use_caller, const std::string& name) : m_name(name) {
    YXTWebCpp_ASSERT(threads > 0);
    if (use_caller) {//将当前线程也用于调度协程
        --threads;
        Thread::SetName(m_name);//给当前线程设置名字,名字就为调度器的名字

        t_scheduler = this;
        Fiber::GetThis();//创建当前线程的主协程块
        m_rootFiber.reset(new Fiber(std::bind(&Scheduler::run, this), 0, true));//创建一个rootFiber,调度协程（该线程的子协程）
        t_scheduler_fiber = m_rootFiber.get();
        m_rootThreadID = GetThreadId();//返回当前线程的唯一标识ID
        m_threadIDs.push_back(m_rootThreadID);//放进线程池ID数组
    }
    m_threadCount = threads;//设置线程池线程数量
}

Scheduler::~Scheduler() {
    YXTWebCpp_ASSERT(stopping());//表示已经停止了
    if (GetThis() == this) {
        t_scheduler = nullptr;
    }
}

void Scheduler::start() {
    ScopedLockImpl<Mutex> guard(m_mutex);
    if (m_stopping == false) {
        return;//已经开启
    }
    m_stopping = false;
    m_threads.resize(m_threadCount);//扩容线程池
    //开启线程池
    for (size_t i = 0; i < m_threadCount; ++i) {
        m_threads[i].reset(new Thread(std::bind(&Scheduler::run, this), m_name + "_" + std::to_string(i)));//创建线程池
        m_threadIDs.push_back(m_threads[i]->getThreadId());//线程唯一标识ID push进 m_threadIDs
    }
}

void Scheduler::stop() {//正常清空下由主线程停止调度器
    m_stopping = true;
    if (stopping()) {
        return;
    }

    for (size_t i = 0; i < m_threadCount; ++i) {
        tickle();
    }
    
    if (m_rootFiber && !stopping()) {//如果use_caller == true 且还无法停止
        m_rootFiber->call();//主线程也切换到m_rootFiber这个协程上执行，将协程任务队列上的任务消耗完
    }
    
    std::vector<std::shared_ptr<Thread> > thrs;
    {
        ScopedLockImpl<Mutex> guard(m_mutex);
        thrs.swap(m_threads);//获得线程池
    }
    for (auto& i : thrs) {
        i->join();//等待线程池的线程都结束
    }
}

void Scheduler::run() {
    t_scheduler = this;//开启了新线程的时候, 当前前线程的t_scheduler指向本调度器
    Fiber::GetThis();//开启了新线程的时候, 为当前线程创建一个主协程块
    if (GetThreadId() != m_rootThreadID) {
        t_scheduler_fiber = Fiber::GetThis().get();
    }
    std::shared_ptr<Fiber> idle_fiber(new Fiber(std::bind(&Scheduler::idle, this)));//空闲协程块
    //以下两个变量用于取协程队列中的任务
    std::shared_ptr<Fiber> cb_fiber;
    FiberAndThread ft;
    while (true) {
        ft.reset();//重置ft
        bool tickle_me = false;
        {
            ////一次只能有一个线程遍历协程队列
            ScopedLockImpl<Mutex> guard(m_mutex);
            auto it = m_fibers.begin();
            while (it != m_fibers.end()) {
                if (it->threadID != -1 && it->threadID != GetThreadId()) {//如果该协程任务指定了线程ID，并且不是本线程，continue
                    ++it;
                    tickle_me = true;
                    continue;
                }
                YXTWebCpp_ASSERT(it->fiber || it->cb);
                //从协程队列取出该任务
                ft = *it;
                m_fibers.erase(it++);
                ++m_activeThreadCount;//激活线程数加1（标识该线程激活了）
                break;
            }
            tickle_me |= (it != m_fibers.end());//tickle_me为true，表示有任务；为false，表示没有协程任务
        }
        
        if (tickle_me) {//如果协程任务队列还有协程任务，调用tickle函数
            tickle();
        }

        if (ft.fiber) {
            ft.fiber->swapIn();//该线程调度该协程
            --m_activeThreadCount;//表示该线程已经调度完该协程，激活线程数减1
        } else if (ft.cb) {
            cb_fiber.reset(new Fiber(ft.cb));
            cb_fiber->swapIn();//该线程调度该协程
            --m_activeThreadCount;//表示该线程已经调度完该协程，激活线程数减1
            cb_fiber.reset();
        }
        if (idle_fiber->getState() == Fiber::TERM) {
            break;
        }
        ++m_idleThreadCount;
        idle_fiber->swapIn();//切换为空闲协程
        --m_idleThreadCount;
        
    }
}

void Scheduler::idle() {
    YXTWebCpp_LOG_INFO(g_logger) << "idle";
    while (!stopping()) {
        Fiber::YieldToHold();
    }
}

bool Scheduler::stopping() {
    ScopedLockImpl<Mutex> guard(m_mutex);
    return m_stopping && m_fibers.empty() && m_activeThreadCount == 0;
}

void Scheduler::tickle() {
    YXTWebCpp_LOG_INFO(g_logger) << "tickle";
}


//将当前的协程切换为另外一个线程调度
void Scheduler::switchTo(int Thread) {
    YXTWebCpp_ASSERT(GetThis() != nullptr);
    if (GetThis() == this && (Thread == -1 || Thread == GetThreadId()) ) {
        return;
    }
    schedule(Fiber::GetThis(), Thread);
    Fiber::YieldToHold();
}

std::ostream& Scheduler::dump(std::ostream& os) {
    os << "[Scheduler name=" << m_name
        << " size=" << m_threadCount
        << " active_count=" << m_activeThreadCount
        << " idle_count=" << m_idleThreadCount
        << " stopping=" << m_stopping
        << " ]" << std::endl << "    ";
    for (size_t i = 0; i < m_threadIDs.size(); ++i) {
        if (i) {
            os << ",";
        }
        os << m_threadIDs[i];
    }
    return os;
}

//静态成员函数
Scheduler* Scheduler::GetThis() {
    return t_scheduler;
}

Fiber* Scheduler::GetMainFiber() {
    return t_scheduler_fiber;
}

/////////////////////////////////////////////////////////////////////////////////
SchedulerSwitcher::SchedulerSwitcher(Scheduler* target) {
    m_caller = Scheduler::GetThis();
    if (target) {
        target->switchTo();
    }
}

SchedulerSwitcher::~SchedulerSwitcher() {
    if (m_caller) {
        m_caller->switchTo();
    }
}

}