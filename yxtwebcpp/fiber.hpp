/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-10-03 13:02:32
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-10-08 12:07:48
 * @FilePath: /yxtweb-cpp/yxtwebcpp/fiber.hpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef _FIBER_HPP_
#define _FIBER_HPP_

#include <memory>
#include <functional>
#include <ucontext.h>

namespace YXTWebCpp {

class Scheduler;

class Fiber : public std::enable_shared_from_this<Fiber> {
public:
enum State {
    INIT,//初始状态
    EXEC,//执行状态
    HOLD,//挂起状态
    TERM,//终止状态
};

private:
    Fiber();

public:
    Fiber(std::function<void()> cb, size_t stackszie = 0, bool use_caller = false);
    ~Fiber();
    void reset(std::function<void()> cb);//重置协程的挂接执行函数，并且设置m_state

    void swapIn();
    void call();//主协程 -> 当前协程

    uint64_t getId() const { return m_id; }
    State getState() const { return m_state; }

private:
    void swapOut();
    void back();//当前协程 ->主协程

public:
    static void SetThis(Fiber *f);//设置当前线程的运行协程
    static std::shared_ptr<Fiber> GetThis();//返回当前线程所执行的协程（如果没有，返回主协程（新创建））
    static void YieldToHold();//当前协程->主协程 state->Hold
    static void MainFunc();//执行完成返回到线程主协程
    static void CallMainFunc();
    static uint64_t TotalFibers();//总的协程数量
    static uint64_t GetFiberId();//获取协程ID

private:
    static Fiber* FunHelper();

private:
    uint64_t m_id = 0;//协程id
    State m_state = INIT;//协程状态
    uint32_t m_stacksize = 0;//栈的大小
    void* m_stack = nullptr;//指向该协程的栈
    std::function<void()> m_cb;//该协程挂接的执行函数
    ucontext_t m_context;//该协程的上下文
    bool m_use_caller = false;
};

}

#endif