/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-10-09 15:36:35
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-10-13 14:44:02
 * @FilePath: /yxtweb-cpp/yxtwebcpp/timer.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "timer.hpp"
#include "util.hpp"
#include "log.hpp"
static std::shared_ptr<YXTWebCpp::Logger> g_logger = YXTWebCpp_LOG_NAME("system");

namespace YXTWebCpp {

Timer::Timer(uint64_t ms, std::function<void()> cb, bool recurring, TimerManager* manager) 
    :m_recurring(recurring) 
    ,m_ms(ms)
    ,m_cb(cb)
    ,m_manager(manager){
    m_next = GetCurrentMS() + m_ms;
}

Timer::Timer(uint64_t next) : m_next(next) {}

bool Timer::cancel() {
    WriteScopedLockImpl<RWMutex> guard(m_manager->m_mutex);
    if (m_cb) {
        m_cb = nullptr;
        auto it = m_manager->m_timers.find(shared_from_this());
        m_manager->m_timers.erase(it);//重定时器列表中删除自己
        return true;
    }
    return false;
}

bool Timer::refresh() {
    WriteScopedLockImpl<RWMutex> guard(m_manager->m_mutex);
    if (!m_cb) {
        return false;
    }
    auto it = m_manager->m_timers.find(shared_from_this());
    if (it == m_manager->m_timers.end()) {
        return false;
    }
    m_manager->m_timers.erase(it);
    m_next = GetCurrentMS() + m_ms;
    m_manager->m_timers.insert(shared_from_this());
    return true;
}

bool Timer::reset(uint64_t ms, bool from_now) {
    if (ms == m_ms && !from_now) {
        return true;
    }
    {
        WriteScopedLockImpl<RWMutex> guard(m_manager->m_mutex);
        if (!m_cb) {
            return false;
        }
        auto it = m_manager->m_timers.find(shared_from_this());
        if (it == m_manager->m_timers.end()) {
            return false;
        }
        m_manager->m_timers.erase(it);
    }
    uint64_t start = 0;
    if (from_now) {
        start = GetCurrentMS();
    } else {
        start = m_next - m_ms;
    }
    m_ms = ms;
    m_next = start + m_ms;
    m_manager->addTimer(shared_from_this());
    return true;
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
    m_previousTime = GetCurrentMS();
}

TimerManager::~TimerManager() {}
    
std::shared_ptr<Timer> TimerManager::addTimer(uint64_t ms, std::function<void()> cb, bool recurring) {
    std::shared_ptr<Timer> timer(new Timer(ms, cb, recurring, this));
    addTimer(timer);
    return timer;
}

static void Ontimer(std::weak_ptr<void> weak_cond, std::function<void()> cb) {//条件定时器的辅助函数
    std::shared_ptr<void> tmp = weak_cond.lock();
    if (tmp) {
        cb();
    }
}

std::shared_ptr<Timer> TimerManager::addConditionTimer(uint64_t ms, std::function<void()> cb, std::weak_ptr<void> weak_cond, bool recurring) {
    return addTimer(ms, std::bind(&Ontimer, weak_cond, cb), recurring);
}

uint64_t TimerManager::getNextTimer() {//获取下一个定时器即将触发的时间
    ReadScopedLockImpl<RWMutex> guard(m_mutex);
    if (m_timers.empty()) {
        return ~0uLL;
    }

    std::shared_ptr<Timer> next = *m_timers.begin();
    uint64_t now_ms = GetCurrentMS();
    if (now_ms >= next->m_next) {//定时器列表的第一个定时器已经超时了
        return 0;
    } else {
        return next->m_next - now_ms;
    }
}

void TimerManager::listExpiredCb(std::vector<std::function<void()> >& cbs) {
    uint64_t now_ms = GetCurrentMS();
    std::vector<std::shared_ptr<Timer> > expiredTimers;//用于放置超时的定时器
    {
        ReadScopedLockImpl<RWMutex> guard(m_mutex);
        if (m_timers.empty()) {
            return;
        }
    }

    WriteScopedLockImpl<RWMutex> guard(m_mutex);
    if (m_timers.empty()) {
        return;
    }
    bool rollover = detectClockRollover(now_ms);
    if (!rollover && (*m_timers.begin())->m_next > now_ms) {
        return;
    }

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
        if (timer->m_recurring) {//如果定时器可以循环的，重新加入定时器列表
            timer->m_next += timer->m_ms;
            m_timers.insert(timer);
        }
    }
}


void TimerManager::addTimer(std::shared_ptr<Timer> val) {
    m_mutex.rdlock();
    auto it = m_timers.insert(val).first;//it为指向插入的定时器的迭代器
    bool at_front = (it == m_timers.begin());
    m_mutex.unlock();

    if (at_front) {
        onTimerInsertedAtFront();//纯虚函数
    }
}

bool TimerManager::detectClockRollover(uint64_t now_ms) {
    bool rollover = false;
    if (now_ms < m_previousTime && now_ms < (m_previousTime - 60 * 60 * 1000)) {
        rollover == true;
    }
    m_previousTime = now_ms;
    return rollover;
}

}