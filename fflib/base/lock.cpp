#include "base/lock.h"

using namespace ff;

mutex_t::mutex_t()
{
    pthread_mutex_init(&m_mutex, NULL);
}

mutex_t::~mutex_t()
{
    pthread_mutex_destroy(&m_mutex);
}

bool mutex_t::lock()
{
    if (pthread_mutex_lock(&m_mutex))
    {
        return false;
    }
    return true;
}

bool mutex_t::time_lock(int us_)
{
    timespec ts = {us_/(1000000), us_*(1000)};
    if (pthread_mutex_timedlock(&m_mutex, &ts))
    {
        return false;
    }
    return true;
}

bool mutex_t::unlock()
{
    if (pthread_mutex_unlock(&m_mutex))
    {
        return false;
    }
    return true;
}

bool mutex_t::try_lock()
{
    if (pthread_mutex_trylock(&m_mutex))
    {
        return false;
    }
    return true;
}


rw_mutex_t::rw_mutex_t()
{
    pthread_rwlock_init(&m_rwlock, NULL);
}

rw_mutex_t::~rw_mutex_t()
{
    pthread_rwlock_destroy(&m_rwlock);
}

bool rw_mutex_t::rd_lock()
{
    return 0 == pthread_rwlock_rdlock(&m_rwlock);
}

bool rw_mutex_t::wr_lock()
{
    return 0 == pthread_rwlock_wrlock(&m_rwlock);
}

bool rw_mutex_t::unlock()
{
    return 0 == pthread_rwlock_unlock(&m_rwlock);
}

condition_var_t::condition_var_t(mutex_t& mutex_):
    m_mutex(mutex_)
{
    pthread_cond_init(&m_cond, NULL);
}

condition_var_t::~condition_var_t()
{
    pthread_cond_destroy(&m_cond);
}

bool condition_var_t::wait()
{
    return 0 == pthread_cond_wait(&m_cond, &m_mutex.get_mutex());
}

bool condition_var_t::time_wait(int us_)
{
    timespec ts = {us_/(1000000), us_*(1000)};
    return 0 == pthread_cond_timedwait(&m_cond, &m_mutex.get_mutex(), &ts);
}

bool condition_var_t::signal()
{
    return 0 == pthread_cond_signal(&m_cond);
}

bool condition_var_t::broadcast()
{
    return 0 == pthread_cond_broadcast(&m_cond);
}

spin_lock_t::spin_lock_t()
{
    pthread_spin_init(&spinlock, 0);
}
spin_lock_t::~spin_lock_t()
{
    pthread_spin_destroy(&spinlock);
}
void spin_lock_t::lock()
{
    pthread_spin_lock(&spinlock);
}
void spin_lock_t::unlock()
{
    pthread_spin_unlock(&spinlock);
}
bool spin_lock_t::try_lock()
{
    if (pthread_spin_trylock(&spinlock))
    {
        return false;
    }
    return true;
}

spin_lock_guard_t::spin_lock_guard_t(spin_lock_t& lock_):
    lock(lock_)
{
    lock.lock();
}
spin_lock_guard_t::~spin_lock_guard_t()
{
    lock.unlock();
}
