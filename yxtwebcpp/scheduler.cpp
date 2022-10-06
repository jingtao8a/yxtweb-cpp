/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-10-06 11:36:33
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-10-06 20:42:32
 * @FilePath: /yxtweb-cpp/yxtwebcpp/scheduler.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "scheduler.hpp"
#include "log.hpp"
#include "macro.hpp"

namespace YXTWebCpp {

std::shared_ptr<Logger> g_logger = YXTWebCpp_LOG_NAME("system");

static thread_local Scheduler* t_scheduler = nullptr;
//指向当前线程调度的协程，加上Fiber模块的t_fiber和t_thread_fiber,每个线程可以记录三个协程的上下文信息
static thread_local Fiber* t_scheduler_fiber = nullptr;

Scheduler::Scheduler(size_t threads = 1, bool use_caller = true, const std::string& name = "") : m_name(name) {
    YXTWebCpp_ASSERT(threads > 0);
    if (use_caller) {
        Fiber::GetThis();//创建当前线程的主协程块
        --threads;
        YXTWebCpp_ASSERT(GetThis() == nullptr);
        t_scheduler = this;//t_scheduler指向当前调度器

        //创建调度协程（该线程的子协程）
        m_rootFiber.reset(new Fiber(std::bind(&Scheduler::run, this), 0, true));//创建一个rootFiber
        t_scheduler_fiber = m_rootFiber.get();

        Thread::SetName(m_name);//给当前线程设置名字
        m_rootThreadID = GetThreadId();//返回当前线程的唯一标识ID
        m_threadIDs.push_back(m_rootThreadID);//放进线程ID数组
    } else {
        m_rootThreadID = -1;
    }
    m_threadCount = threads;//设置线程数量
}

Scheduler::~Scheduler() {
    YXTWebCpp_ASSERT(m_stopping);
    if (GetThis() == this) {
        t_scheduler = nullptr;
    }
}

void Scheduler::start() {
    ScopedLockImpl<Mutex> guard(m_mutex);
    if (!m_stopping) {
        return;
    }
    m_stopping = false;
    YXTWebCpp_ASSERT(m_threads.empty());//assert线程池为空

    m_threads.resize(m_threadCount);//扩容线程池
    //开启线程池
    for (size_t i = 0; i < m_threadCount; ++i) {
        m_threads[i].reset(new Thread(std::bind(&Scheduler::run, this), m_name + "_" + std::to_string(i)));//创建线程池
        m_threadIDs.push_back(m_threads[i]->getThreadId());//并且将线程ID push进 m_threadIDs
    }
}

void Scheduler::stop() {
    m_autostop = true;
    if (m_rootFiber && m_threadCount == 0 //只有一个主线程调度的情况，并且use_caller == true
        && (m_rootFiber->getState() == Fiber::TERM || m_rootFiber->getState() == Fiber::INIT)) {
            YXTWebCpp_LOG_INFO(g_logger) << this << "stopped";
            m_stopping = true;
            if (stopping()) {
                return;
            }
    }
    
    bool exit_on_this_fiber = false;//在这个协程上停止

    m_stopping = true;
    for (size_t i = 0; i < m_threadCount; ++i) {
        tickle();
    }

    if (m_rootFiber) {
        tickle();
        if (!stopping()) {
            m_rootFiber->call();//切换到m_rootFiber这个协程上执行
        }
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
    YXTWebCpp_LOG_DEBUG(g_logger) << m_name << "run";
    setThis();
    if (GetThreadId() != m_rootThreadID) {//如果不是主线程
        t_scheduler_fiber = Fiber::GetThis().get();//为当前线程创建一个主协程块，并且用t_scheduler_fiber指向它，当前线程的调度协程
    }
    std::shared_ptr<Fiber> idle_fiber(new Fiber(std::bind(&Scheduler::idle, this)));//空闲协程块
    std::shared_ptr<Fiber> cb_fiber;
    FiberAndThread ft;
    while (true) {
        ft.reset();
        bool tickle_me = false;
        bool is_active = false;
        {
            //遍历协程队列
            ScopedLockImpl<Mutex> guard(m_mutex);//一次只能有一个线程遍历协程队列
            auto it = m_fibers.begin();
            while (it != m_fibers.end()) {
                if (it->threadID != -1 && it->threadID != GetThreadId()) {//如果指定了线程ID，并且不是本线程，continue
                    ++it;
                    tickle_me = true;
                    continue;
                }
                YXTWebCpp_ASSERT(it->fiber || it->cb);
                if (it->fiber && it->fiber->getState() == Fiber::EXEC) {//如果该协程正在执行，continue
                    ++it;
                    continue;
                }

                ft = *it;
                m_fibers.erase(it++);//从协程队列删除该任务
                ++m_activeThreadCount;//激活线程数加1（标识该线程激活了）
                is_active = true;
                break;
            }
            tickle_me |= (it != m_fibers.end());//tickle_me为true，表示有任务；为false，表示没有协程任务
        }
        if (tickle_me) {//如果有协程任务，调用tickle函数
            tickle();
        }

    }
}

void Scheduler::setThis() {
    t_scheduler = this;
}

void Scheduler::tickle() {
    YXTWebCpp_LOG_INFO(g_logger) << "tickle";
}

bool Scheduler::stopping() {
    ScopedLockImpl<Mutex> guard(m_mutex);
    return m_autostop && m_stopping && m_fibers.empty() && m_activeThreadCount == 0;
}

void Scheduler::idle() {
    YXTWebCpp_LOG_INFO(g_logger) << "idle";
    while (!stopping()) {
        Fiber::YieldToHold();
    }
}

//切换线程
void Scheduler::switchTo(int Thread) {
    YXTWebCpp_ASSERT(Scheduler::GetThis() != nullptr);
    if (GetThis() == this && (Thread == -1 || Thread == GetThreadId()) ) {
        return;
    }
    schedule(GetThis(), Thread);
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