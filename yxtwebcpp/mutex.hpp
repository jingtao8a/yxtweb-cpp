#ifndef _MUTEX_HPP_
#define _MUTEX_HPP_

#include <semaphore.h>
#include <stdint.h>
#include <pthread.h>
#include <atomic>
#include <stdexcept>
#include "nocopyable.hpp"

namespace YXTWebCpp {

class Semaphore : public Nocopyable {
public:
    Semaphore(uint32_t count = 0);//信号量初始值
    ~Semaphore();
    void wait();
    void notify();
private:
    sem_t m_semaphore;
};

template<class T>
class ScopedLockImpl {
public:
    ScopedLockImpl(T& mutex):m_mutex(mutex){
        this->lock();
    }

    ~ScopedLockImpl() {
        unlock();
    }

    void lock() {
        if (!m_locked) {
            m_mutex.lock();
            m_locked = true;
        }
    }

    void unlock() {
        if (m_locked) {
            m_mutex.unlock();
            m_locked = false;
        }
    }
protected:
    T& m_mutex;
    bool m_locked;
};

template <class T>
class ReadScopedLockImpl {
public:
    ReadScopedLockImpl(T& mutex): m_mutex(mutex) {
        lock();
    }

    ~ReadScopedLockImpl() {
        unlock();
    }

    void lock() {
        if (!m_locked) {
            m_mutex.rdlock();
            m_locked = true;
        }
    }

    void unlock() {
        if (m_locked) {
            m_mutex.unlock();
            m_locked = false;
        }
    }
protected:
    T& m_mutex;
    bool m_locked;
};

template <class T>
class WriteScopedLockImpl {
public:
    WriteScopedLockImpl(T& mutex): m_mutex(mutex) {
        lock();
    }

    ~WriteScopedLockImpl() {
        unlock();
    }

    void lock() {
        if (!m_locked) {
            m_mutex.wrlock();
            m_locked = true;
        }
    }

    void unlock() {
        if (m_locked) {
            m_mutex.unlock();
            m_locked = false;
        }
    }
protected:
    T& m_mutex;
    bool m_locked;
};

class Mutex : public Nocopyable{
public:
    Mutex() {
        pthread_mutex_init(&m_mutex, nullptr);
    }
    ~Mutex() {
        pthread_mutex_destroy(&m_mutex);
    }
    void lock() {
        pthread_mutex_lock(&m_mutex);
    }
    void unlock() {
        pthread_mutex_unlock(&m_mutex);
    }
private:
    pthread_mutex_t m_mutex;
};

class RWMutex : public Nocopyable {
public:
    RWMutex() {
        int rt = pthread_rwlock_init(&m_mutex, nullptr);
        if (rt) {
            throw std::logic_error("pthread_rwlock_init failed");
        }
    }

    void rdlock() {
        pthread_rwlock_rdlock(&m_mutex);
    }

    void wrlock() {
        pthread_rwlock_wrlock(&m_mutex);
    }

    void unlock() {
        pthread_rwlock_unlock(&m_mutex);
    }

    ~RWMutex() {
        pthread_rwlock_destroy(&m_mutex);
    }
private:
    pthread_rwlock_t m_mutex;

};

class Spinlock : public Nocopyable {
public:
    Spinlock() {
        pthread_spin_init(&m_mutex, 0);
    }

    ~Spinlock() {
        pthread_spin_destroy(&m_mutex);
    }

    void lock() {
        pthread_spin_lock(&m_mutex);
    }

    void unlock() {
        pthread_spin_unlock(&m_mutex);
    }
private:
    pthread_spinlock_t m_mutex;
};

class CASLock : public Nocopyable {
public:
    CASLock() {
        m_mutex.clear();
    }
    ~CASLock() {}

    void lock() {
        while (std::atomic_flag_test_and_set_explicit(&m_mutex, std::memory_order_acquire));
    }

    void unlock() {
        std::atomic_flag_clear_explicit(&m_mutex, std::memory_order_release);
    }
private:
    volatile std::atomic_flag m_mutex;
};



}
#endif