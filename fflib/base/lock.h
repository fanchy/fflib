#ifndef _LOCK_H_
#define _LOCK_H_

#include<pthread.h>

namespace ff {

class mutex_t
{
public:
    mutex_t();
    ~mutex_t();
    bool lock();
    bool time_lock(int us_);
    bool unlock();
    bool try_lock();

    pthread_mutex_t& get_mutex() {return m_mutex;}

private:
    pthread_mutex_t              m_mutex;
};

class rw_mutex_t
{
public:
    rw_mutex_t();
    ~rw_mutex_t();
    bool rd_lock();
    bool wr_lock();
    bool unlock();
private:
    pthread_rwlock_t m_rwlock;
};

class condition_var_t
{
public:
    condition_var_t(mutex_t& mutex_);
    ~condition_var_t();
    bool wait();
    bool time_wait(int us_);
    bool signal();
    bool broadcast();

private:
    mutex_t&                         m_mutex;
    pthread_cond_t                m_cond;
};

class lock_guard_t
{
public:
    lock_guard_t(mutex_t& mutex_):
        m_flag(false),
        m_mutex(mutex_)
    {
        m_mutex.lock();
        m_flag = true;
    }
    ~lock_guard_t()
    {
        if (is_locked())
        {
            unlock();
        }
    }

    bool unlock()
    {
        m_flag = false;
        return m_mutex.unlock();
    }

    bool is_locked() const { return m_flag; }
private:
    bool               m_flag;
    mutex_t&      m_mutex;
};

class rd_lock_guard_t
{
public:
    rd_lock_guard_t(rw_mutex_t& mutex_):
        m_flag(false),
        m_mutex(mutex_)
    {
        m_mutex.rd_lock();
        m_flag = true;
    }
    ~rd_lock_guard_t()
    {
        if (is_locked())
        {
            unlock();
        }
    }

    bool unlock()
    {
        m_flag = false;
        return m_mutex.unlock();
    }

    bool is_locked() const { return m_flag; }
private:
    bool                    m_flag;
    rw_mutex_t&      m_mutex;
};

class wr_lock_guard_t
{
public:
    wr_lock_guard_t(rw_mutex_t& mutex_):
        m_flag(false),
        m_mutex(mutex_)
    {
        m_mutex.wr_lock();
        m_flag = true;
    }
    ~wr_lock_guard_t()
    {
        if (is_locked())
        {
            unlock();
        }
    }

    bool unlock()
    {
        m_flag = false;
        return m_mutex.unlock();
    }

    bool is_locked() const { return m_flag; }
private:
    bool                    m_flag;
    rw_mutex_t&      m_mutex;
};


class spin_lock_t
{
public:
    spin_lock_t();
    ~spin_lock_t();
    void lock();
    void unlock();
    bool try_lock();
private:
    pthread_spinlock_t spinlock;
};

class spin_lock_guard_t
{
public:
    spin_lock_guard_t(spin_lock_t& lock_);
    ~spin_lock_guard_t();

private:
    spin_lock_t& lock;
};
}
#endif

