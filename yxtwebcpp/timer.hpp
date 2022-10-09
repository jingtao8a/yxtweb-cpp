/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-10-09 15:36:45
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-10-09 19:13:55
 * @FilePath: /yxtweb-cpp/yxtwebcpp/timer.hpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef _TIMER_HPP_
#define _TIMER_HPP_

#include "thread.hpp"
#include <set>

namespace YXTWebCpp {

class Timer {
friend class TimerManager;
private:
    Timer(uint64_t ms, std::function<void()> cb, bool recurring, TimerManager* manager);
private:
    bool m_recurring = false;//是否循环定时器
    uint64_t m_ms = 0;//执行周期
    uint64_t m_next = 0;//下一次触发定时器精确的执行时间
    std::function<void()> m_cb;//定时执行的函数对象
    TimerManager *m_manager = nullptr;//该定时器被某个TimerManager管理
private:
    struct Comparator {
        bool operator()(const std::shared_ptr<Timer>& lhs, const std::shared_ptr<Timer>& rhs );
    };
    
};

//抽象类
class TimerManager {
friend class Timer;
public:
    TimerManager();
    virtual ~TimerManager();
    std::shared_ptr<Timer> addTimer(uint64_t ms, std::function<void()> cb, bool recurring = false);
    std::shared_ptr<Timer> addConditionTimer(uint64_t ms, std::function<void()> cb, std::weak_ptr<void> weak_cond, bool recurring = false);
    uint64_t TimerManager::getNextTimer();
    void listExpiredCb(std::vector<std::function<void()> >& cbs);
protected:
    virtual void onTimerInsertedAtFront() = 0;

private:
    RWMutex m_mutex;
    std::set<std::shared_ptr<Timer>, Timer::Comparator> m_timers;
};

}

#endif