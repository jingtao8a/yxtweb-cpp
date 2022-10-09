/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-10-09 15:36:35
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-10-09 19:22:25
 * @FilePath: /yxtweb-cpp/yxtwebcpp/timer.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "timer.hpp"
#include "util.hpp"

namespace YXTWebCpp {

Timer::Timer(uint64_t ms, std::function<void()> cb, bool recurring, TimerManager* manager) 
    :m_recurring(recurring) 
    ,m_ms(ms)
    ,m_cb(cb)
    ,m_manager(manager){
    m_next = GetCurrentMS() + m_ms;
}

bool Timer::Comparator::operator()(const std::shared_ptr<Timer>& lhs, const std::shared_ptr<Timer>& rhs ) {
    if (!lhs && !rhs) {
        return false;
    }
    if (!lhs) {
        return true;
    }

    if (!rhs) {
        return false;
    }

    if (lhs->m_next < rhs->m_next) {
        return true;
    }
    if (lhs->m_next > rhs->m_next) {
        return false;
    }
    return lhs.get() < rhs.get();
}


TimerManager::TimerManager() {

}

TimerManager::~TimerManager() {

}
    
std::shared_ptr<Timer> TimerManager::addTimer(uint64_t ms, std::function<void()> cb, bool recurring = false) {
    std::shared_ptr<Timer> timer(new Timer(ms, cb, recurring, this));
    m_mutex.rdlock();
    auto it = m_timers.insert(timer).first;//it为指向插入的定时器的迭代器
    bool at_front = (it == m_timers.begin());
    m_mutex.unlock();

    if (at_front) {
        onTimerInsertedAtFront();//纯虚函数
    }
}

static void Ontimer(std::weak_ptr<void> weak_cond, std::function<void()> cb) {
    std::shared_ptr<void> tmp = weak_cond.lock();
    if (tmp) {
        cb();
    }
}

std::shared_ptr<Timer> TimerManager::addConditionTimer(uint64_t ms, std::function<void()> cb, std::weak_ptr<void> weak_cond, bool recurring = false) {
    return addTimer(ms, std::bind(&Ontimer, weak_cond, cb), recurring);
}

uint64_t TimerManager::getNextTimer() {
    ReadScopedLockImpl<RWMutex> guard(m_mutex);
    if (m_timers.empty()) {
        return ~0uL;
    }

    std::shared_ptr<Timer> next = *m_timers.begin();
    uint64_t now_ms = GetCurrentMS();
    if (now_ms >= next->m_next) {
        return 0;
    } else {
        return next->m_next - now_ms;
    }
}

void TimerManager::listExpiredCb(std::vector<std::function<void()> >& cbs) {
    uint64_t now_ms = GetCurrentMS();
    std::vector<std::shared_ptr<Timer> > expiredTimers;
    {
        ReadScopedLockImpl<RWMutex> guard(m_mutex);
        if (m_timers.empty()) {
            return;
        }
    }

    WriteScopedLockImpl<RWMutex> guard(m_mutex);
    std::shared_ptr<Timer> now_timer(new Timer(now_ms, nullptr, false, nullptr));
    auto it = m_timers.lower_bound(now_timer);
    while (it != m_timers.end() && (*it)->m_next == now_ms) {
        ++it;
    }
    expiredTimers.insert(expiredTimers.begin(), m_timers.begin(), it);
    m_timers.erase(m_timers.begin(), it);
    cbs.reserve(expiredTimers.size());//只修改vector的capacity
    for (auto& timer : expiredTimers) {
        cbs.push_back(timer->m_cb);
        if (timer->m_recurring) {
            // timer->m_next = 
        }
    }
}

}