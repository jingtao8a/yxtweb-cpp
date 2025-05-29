/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-10-03 13:02:25
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-10-08 12:09:21
 * @FilePath: /yxtweb-cpp/yxtwebcpp/fiber.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "fiber.hpp"
#include <atomic>
#include "log.hpp"
#include "config.hpp"
#include "macro.hpp"
#include "scheduler.hpp"



// int swapcontext(ucontext_t *oucp, const ucontext_t *ucp);
// oucp: 指向当前上下文将被保存的 ucontext_t 结构体
// ucp: 指向要切换到的上下文 ucontext_t 结构体

// int getcontext(ucontext_t *ucp);
// ucp: 指向用于存储当前上下文的 ucontext_t 结构体

// void makecontext(ucontext_t *ucp, void (*func)(), int argc, ...);
// func: 新上下文中要执行的函数
// argc: 传递给函数的参数个数
// ...: 实际参数列表（均为 int 类型)

namespace YXTWebCpp {

static std::shared_ptr<Logger> g_logger = YXTWebCpp_LOG_NAME("system");

static std::atomic<uint64_t> s_fiber_id(0);//Fiber对象的ID号
static std::atomic<uint64_t> s_fiber_count(0);//Fiber对象的数量

static thread_local Fiber* t_fiber = nullptr;//当前线程运行时，指向的Fiber对象
static thread_local std::shared_ptr<Fiber> t_threadFiber = nullptr;//指向主协程对应的Fiber对象

static std::shared_ptr<ConfigVar<uint32_t> > g_fiber_stack_size = Config::Lookup<uint32_t>("fiber.stack_size", 128 * 1024, "fiber stack size");

Fiber::Fiber() {
    m_state = EXEC;//运行态
    ++s_fiber_count;
    if (getcontext(&m_context)) {
        YXTWebCpp_ASSERT2(false, "getcontext");
    }
    YXTWebCpp_LOG_DEBUG(g_logger) << "Fiber::Fiber main";
}

Fiber::Fiber(std::function<void()> cb, size_t stacksize, bool use_caller) 
    :m_id(++s_fiber_id)
    ,m_cb(cb) 
    ,m_use_caller(use_caller) {
    ++s_fiber_count;

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
    if (!use_caller) { // 设置子协程入口执行函数
        makecontext(&m_context, &Fiber::MainFunc, 0);
    } else { // 设置主协程入口函数
        makecontext(&m_context, &Fiber::CallMainFunc, 0);
    }
    YXTWebCpp_LOG_DEBUG(g_logger) << "Fiber::Fiber id = " << m_id;
}

Fiber::~Fiber() {
    --s_fiber_count;
    if (m_stack) {//如果是非主协程块对象析构
        YXTWebCpp_ASSERT(m_state == TERM || m_state == INIT);
        free(m_stack);//释放申请的栈空间
    } else {//如果是主协程块析构
        YXTWebCpp_ASSERT(m_state == EXEC);
        if (t_fiber == this) {//如果线程当前t_fiber指向主协程对象
            SetThis(nullptr);//设置为nullptr
        }
    }
    YXTWebCpp_LOG_DEBUG(g_logger) << "Fiber::~Fiber id=" << m_id << " total=" << s_fiber_count;
}

void Fiber::reset(std::function<void()> cb) {//重置协程的挂接执行函数，并且设置m_state
    YXTWebCpp_ASSERT(m_stack);
    YXTWebCpp_ASSERT(m_state == TERM || m_state == INIT);
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

// 以下函数在use_caller==false时使用
void Fiber::swapIn() { // 主协程 -> 当前协程
    SetThis(this);
    YXTWebCpp_ASSERT(m_state != EXEC);
    m_state = EXEC;
    if (swapcontext(&(Scheduler::GetMainFiber()->m_context), &m_context)) {
        YXTWebCpp_ASSERT2(false, "swapcontext");
    }
}

void Fiber::swapOut() { // 当前协程 -> 主协程
    SetThis(Scheduler::GetMainFiber());
    if (swapcontext(&m_context, &(Scheduler::GetMainFiber()->m_context))) {
        YXTWebCpp_ASSERT2(false, "swapcontext")
    }
}

// 以下函数在use_caller==true时使用
void Fiber::call() {//主协程 -> 当前协程
    SetThis(this);
    YXTWebCpp_ASSERT(m_state != EXEC);
    m_state = EXEC;
    if (swapcontext(&(t_threadFiber->m_context), &m_context)) {
        YXTWebCpp_ASSERT2(false, "swapcontext");
    }
}

void Fiber::back() {//当前协程 -> 主协程
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
    if (!t_fiber) {
        t_threadFiber.reset(new Fiber);//给主协程创建一个Fiber对象，并用t_fiber指向它
        SetThis(t_threadFiber.get());
    }
    return t_fiber->shared_from_this();
}

void Fiber::YieldToHold() {//将当前协程设置为HOLD,当前协程 -> 主协程
    auto cur = GetThis();
    cur->m_state = HOLD;
    if (!cur->m_use_caller) {
        cur->swapOut();
    } else {
        cur->back();
    }
}

void Fiber::MainFunc() {// usecaller == false
    Fiber* raw_ptr = FunHelper();
    raw_ptr->swapOut();
    YXTWebCpp_ASSERT2(false, "never reach")
}

void Fiber::CallMainFunc() {// usecaller == true
    Fiber* raw_ptr = FunHelper();
    raw_ptr->back();
    YXTWebCpp_ASSERT2(false, "never reach");
}

Fiber* Fiber::FunHelper() {
    std::shared_ptr<Fiber> cur = GetThis();//cur指向的是当前协程Fiber
    cur->m_cb();
    cur->m_cb = nullptr;
    cur->m_state = TERM;
    auto raw_ptr = cur.get();
    cur.reset();
    return raw_ptr;
}

uint64_t Fiber::TotalFibers() {//总的协程数量
    return s_fiber_count;
}

uint64_t Fiber::GetFiberId() {
    if (t_fiber) {
        return t_fiber->getId();
    }
    return 0;
}

}