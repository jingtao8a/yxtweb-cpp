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
class ScopedLockImpl : public Nocopyable {
public:
    ScopedLockImpl(T& mutex):m_mutex(mutex){
        m_mutex.lock();
    }

    ~ScopedLockImpl() {
        m_mutex.unlock();
    }

protected:
    T& m_mutex;
};

template <class T>
class ReadScopedLockImpl : public Nocopyable {
public:
    ReadScopedLockImpl(T& mutex): m_mutex(mutex) {
        m_mutex.rdlock();
    }

    ~ReadScopedLockImpl() {
        m_mutex.unlock();
    }

protected:
    T& m_mutex;
};

template <class T>
class WriteScopedLockImpl : public Nocopyable {
public:
    WriteScopedLockImpl(T& mutex): m_mutex(mutex) {
        m_mutex.wrlock();
    }

    ~WriteScopedLockImpl() {
        m_mutex.unlock();
    }
protected:
    T& m_mutex;
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