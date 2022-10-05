/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-10-03 13:02:25
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-10-05 23:28:01
 * @FilePath: /yxtweb-cpp/yxtwebcpp/fiber.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "fiber.hpp"
#include <atomic>
#include "log.hpp"
#include "config.hpp"
#include "macro.hpp"

namespace YXTWebCpp {

static std::shared_ptr<Logger> g_logger = YXTWebCpp_LOG_NAME("system");

static std::atomic<uint64_t> s_fiber_id(0);//Fiber对象的ID号
static std::atomic<uint64_t> s_fiber_count(0);//Fiber对象的数量

static thread_local Fiber* t_fiber = nullptr;//当前线程运行时，指向的Fiber对象
static thread_local std::shared_ptr<Fiber> t_threadFiber = nullptr;//指向主协程对应的Fiber对象

static std::shared_ptr<ConfigVar<uint32_t> > g_fiber_stack_size = Config::Lookup<uint32_t>("fiber.stack_size", 128 * 1024, "fiber stack size");

Fiber::Fiber() {
    m_state = EXEC;//运行态
    SetThis(this);
    if (getcontext(&m_context)) {
        YXTWebCpp_ASSERT2(false, "getcontext");
    }
    ++s_fiber_count;
    YXTWebCpp_LOG_DEBUG(g_logger) << "Fiber::Fiber main";
}

Fiber::Fiber(std::function<void()> cb, size_t stacksize, bool use_caller) 
    :m_id(++s_fiber_id)
    ,m_cb(cb) {
    ++s_fiber_count;
    //构建一个协程Fiber(context) begin
    uint32_t config_stacksize = g_fiber_stack_size->getValue();
    m_stacksize = stacksize ? stacksize : config_stacksize;
    m_stack = malloc(m_stacksize);

    if (getcontext(&m_context)) {
        YXTWebCpp_ASSERT2(false, "getcontext");
    }
    m_context.uc_link = nullptr;
    m_context.uc_stack.ss_sp = m_stack;
    m_context.uc_stack.ss_size = m_stacksize;
    m_context.uc_stack.ss_flags = 0;
    if (!use_caller) {
        makecontext(&m_context, &Fiber::MainFunc, 0);
    } else {
        makecontext(&m_context, &Fiber::CallerMainFunc, 0);
    }
    YXTWebCpp_LOG_DEBUG(g_logger) << "Fiber::Fiber id = " << m_id;
    //构建一个协程Fiber(context) end
}

Fiber::~Fiber() {
    --s_fiber_count;
    if (m_stack) {//如果是非主协程块对象析构
        YXTWebCpp_ASSERT(m_state == TERM 
                        || m_state == EXCEPT
                        || m_state == INIT);
        free(m_stack);
    } else {//如果是主协程块析构
        YXTWebCpp_ASSERT(!m_cb);
        YXTWebCpp_ASSERT(m_state == EXEC);
        if (t_fiber == this) {//如果线程当前t_fiber指向主协程对象
            SetThis(nullptr);//设置为nullptr
        }
    }
    YXTWebCpp_LOG_DEBUG(g_logger) << "Fiber::~Fiber id=" << m_id << " total=" << s_fiber_count;
}

void Fiber::reset(std::function<void()> cb) {//重置协程的挂接执行函数，并且设置m_state
    YXTWebCpp_ASSERT(m_stack);
    YXTWebCpp_ASSERT(m_state == TERM
            || m_state == EXCEPT
            || m_state == INIT);
    m_cb = cb;
    //重新构建Fiber对象(context)
    if(getcontext(&m_context)) {
        YXTWebCpp_ASSERT2(false, "getcontext");
    }

    m_context.uc_link = nullptr;
    m_context.uc_stack.ss_sp = m_stack;
    m_context.uc_stack.ss_size = m_stacksize;
    m_context.uc_stack.ss_flags = 0;
    makecontext(&m_context, &Fiber::MainFunc, 0);
    m_state = INIT;
}

void Fiber::swapIn() {//切换为该协程
    SetThis(this);
    YXTWebCpp_ASSERT(m_state != EXEC);
    m_state = EXEC;
    if (swapcontext(&(t_threadFiber->m_context), &m_context)) {
        YXTWebCpp_ASSERT2(false, "swapcontext");
    }
}

void Fiber::swapOut() {//当前协程挂起
    SetThis(t_threadFiber.get());
    if (swapcontext(&m_context, &t_threadFiber->m_context)) {
        YXTWebCpp_ASSERT2(false, "swapcontext");
    }
}

void Fiber::call() {//将当前主线程挂起，切换为当前协程
    SetThis(this);
    m_state = EXEC;
    if (swapcontext(&t_threadFiber->m_context, &m_context)) {
        YXTWebCpp_ASSERT2(false, "swapcontext");
    }
}

void Fiber::back() {//将当前协程挂起，切换为主线程
    SetThis(t_threadFiber.get());
    if (swapcontext(&m_context, &t_threadFiber->m_context)) {
        YXTWebCpp_ASSERT2(false, "swapcontext");
    }
}

//静态成员函数
void Fiber::SetThis(Fiber *f) {//设置当前线程的运行协程
    t_fiber = f;
}

std::shared_ptr<Fiber> Fiber::GetThis() {//返回当前线程运行的协程Fiber
    if (t_fiber) {//如果t_fiber不为nullptr
        return t_fiber->shared_from_this();
    }
    //说明当前运行的是主线程，
    std::shared_ptr<Fiber> main_fiber(new Fiber);//给主线程创建一个Fiber对象，并用t_fiber指向它
    t_threadFiber = main_fiber;
    return t_fiber->shared_from_this();//返回主协程Fiber对象
}

void Fiber::YieldToReady() {//将当前协程设置为ready，切换为主协程
    auto cur = GetThis();
    cur->m_state = READY;
    cur->swapOut();
}

void Fiber::YieldToHold() {//将当前协程设置为HOLD，
    auto cur = GetThis();
    cur->m_state = HOLD;
    cur->swapOut();
}

uint64_t Fiber::TotalFibers() {//总的协程数量
    return s_fiber_count;
}

void Fiber::MainFunc() {//执行完成返回到线程主协程
    std::shared_ptr<Fiber> cur = GetThis();//cur指向的是当前协程Fiber
    try {
        cur->m_cb();
        cur->m_cb = nullptr;
        cur->m_state = TERM;
    } catch (std::exception& ex) {
        cur->m_state = EXCEPT;
        YXTWebCpp_LOG_ERROR(g_logger) << "Fiber Except: " << ex.what()
            << " fiber_id=" << cur->getId()
            << std::endl
            << BacktraceToString();
    } catch (...) {
        cur->m_state = EXCEPT;
        YXTWebCpp_LOG_ERROR(g_logger) << "Fiber Except"
            << " fiber_id=" << cur->getId()
            << std::endl
            << BacktraceToString();
    }
    auto raw_ptr = cur.get();
    cur.reset();
    raw_ptr->swapOut();
}

void Fiber::CallerMainFunc() {//执行完成返回到线程调度协程
    std::shared_ptr<Fiber> cur = GetThis();//cur指向的是主协程Fiber
    try {
        cur->m_cb();
        cur->m_cb = nullptr;
        cur->m_state = TERM;
    } catch (std::exception& ex) {
        cur->m_state = EXCEPT;
        YXTWebCpp_LOG_ERROR(g_logger) << "Fiber Except: " << ex.what()
            << " fiber_id=" << cur->getId()
            << std::endl
            << BacktraceToString();
    } catch (...) {
        cur->m_state = EXCEPT;
        YXTWebCpp_LOG_ERROR(g_logger) << "Fiber Except"
            << " fiber_id=" << cur->getId()
            << std::endl
            << BacktraceToString();
    }
    auto raw_ptr = cur.get();//注意协程使用的栈实际上是在堆上开辟的空间
    cur.reset();
    raw_ptr->back();
}

uint64_t Fiber::GetFiberId() {
    if (t_fiber) {
        return t_fiber->getId();
    }
    return 0;
}

}