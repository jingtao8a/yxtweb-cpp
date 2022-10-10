/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-10-09 15:36:45
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-10-10 19:53:40
 * @FilePath: /yxtweb-cpp/yxtwebcpp/timer.hpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef _TIMER_HPP_
#define _TIMER_HPP_

#include "thread.hpp"
#include <set>
#include <vector>

namespace YXTWebCpp {

class TimerManager;

class Timer : public std::enable_shared_from_this<Timer> {
friend class TimerManager;
private:
    Timer(uint64_t ms, std::function<void()> cb, bool recurring, TimerManager* manager);
    Timer(uint64_t next);

public:
    bool cancel();//取消定时器（从定时器列表中删除自己）
    bool refresh();//刷新设置定时器的时间
    bool reset(uint64_t ms, bool from_now = true);//重置定时器的周期时间，from_now表示是否从当前时间开始计算

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
    uint64_t getNextTimer();//距离下一次定时器触发的时间
    void listExpiredCb(std::vector<std::function<void()> >& cbs);
    
protected:
    virtual void onTimerInsertedAtFront() = 0;

private:
    void addTimer(std::shared_ptr<Timer> val);
    bool detectClockRollover(uint64_t now_ms);
private:
    RWMutex m_mutex;
    std::set<std::shared_ptr<Timer>, Timer::Comparator> m_timers;
    uint64_t m_previousTime = 0;//上次执行listExpiredCb的时间
};

}

#endif