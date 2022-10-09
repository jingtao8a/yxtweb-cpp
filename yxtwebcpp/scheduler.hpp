/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-10-06 11:36:43
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-10-09 13:09:52
 * @FilePath: /yxtweb-cpp/yxtwebcpp/scheduler.hpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef _SCHEDULER_HPP
#define _SCHEDULER_HPP

#include "thread.hpp"
#include "fiber.hpp"
#include "nocopyable.hpp"
#include <atomic>
#include <vector>
#include <list>

namespace YXTWebCpp {

class Scheduler {
public:
    Scheduler(size_t threads = 1, bool use_caller = true, const std::string& name = "");
    virtual ~Scheduler();

    std::string getName() const { return m_name; }//返回协程调度器的名字

    void start();//开启调度器
    void stop();//停止调度器
    void run();//run函数
    void switchTo(int Thread = -1);
    std::ostream& dump(std::ostream& os);

    //schedule函数用于往调度器里装协程任务
    template<class FiberOrCb>
    void schedule(FiberOrCb fc, int thread = -1) {
        bool need_tickle = false;
        {
            ScopedLockImpl<Mutex> guard(m_mutex);
            need_tickle = scheduleNoLock(fc, thread);
        }
        if (need_tickle) {//为true表示任务队列有任务
            tickle();
        }
    }

private:
    template <class FiberOrCb>
    bool scheduleNoLock(FiberOrCb fc, int thread) {
        bool need_tickle = m_fibers.empty();
        FiberAndThread ft(fc, thread);
        if (ft.fiber || ft.cb) {
            m_fibers.push_back(ft);
        }
        return need_tickle;
    }
protected:
    virtual void tickle();
    virtual bool stopping();
    virtual void idle();
    
    bool hasIdleThreads() { return m_idleThreadCount > 0; }

public:
    static Scheduler* GetThis();
    static Fiber* GetMainFiber();
    
private:
    struct FiberAndThread {
        std::shared_ptr<Fiber> fiber;//协程
        std::function<void()> cb;//协程执行函数
        int threadID;//线程ID

        FiberAndThread(std::shared_ptr<Fiber> f, int thr) : fiber(f), threadID(thr) {}
        FiberAndThread(std::shared_ptr<Fiber>* f, int thr) : threadID(thr) { fiber.swap(*f); }
        FiberAndThread(std::function<void()> f, int thr) : cb(f), threadID(thr) {}
        FiberAndThread(std::function<void()> *f, int thr) : threadID(thr) { cb.swap(*f); }
        FiberAndThread() : threadID(-1) {}

        void reset() {
            fiber = nullptr;
            cb = nullptr;
            threadID = -1;
        }
    }; 

private:
    Mutex m_mutex;
    std::vector<std::shared_ptr<Thread> > m_threads;//线程池
    std::list<FiberAndThread> m_fibers;//协程任务队列
    std::shared_ptr<Fiber> m_rootFiber;//user_caller为true时有效，为调度协程
    std::string m_name;//Scheduler名称

protected:
    std::vector<int> m_threadIDs;//线程池ID数组
    std::atomic<size_t> m_threadCount = {0};//线程数量
    std::atomic<size_t> m_activeThreadCount = {0};//工作线程数量
    std::atomic<size_t> m_idleThreadCount = {0};//空闲线程数量

    bool m_stopping = true;//是否正在停止中
    // bool m_autostop = false;//是否自动停止
    int m_rootThreadID = -1;//主线程ID,当user_caller = true时，不为-1
};

//Scheduler切换器
class SchedulerSwitcher : public Nocopyable {
public:
    SchedulerSwitcher(Scheduler* target = nullptr);
    ~SchedulerSwitcher();
private:
    Scheduler* m_caller;
};

}

#endif