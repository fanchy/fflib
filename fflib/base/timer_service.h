#ifndef _TIMER_SERVICE_H_
#define _TIMER_SERVICE_H_

#include <sys/epoll.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <assert.h>
#include <stdint.h>

#include <list>
#include <map>
using namespace std;

#include "base/thread.h"
#include "base/lock.h"

namespace ff {

class timer_service_t 
{
    struct interupt_info_t
    {
        int pair_fds[2];
        interupt_info_t()
        {
            assert(0 == ::socketpair(AF_LOCAL, SOCK_STREAM, 0, pair_fds));
            ::write(pair_fds[1], "0", 1);
        }
        ~interupt_info_t()
        {
            ::close(pair_fds[0]);
            ::close(pair_fds[1]);
        }
        int read_fd() { return pair_fds[0]; }
        int write_fd() { return pair_fds[1]; }
    };
    struct registered_info_t
    {
        registered_info_t(uint64_t ms_, uint64_t dest_ms_, const task_t& t_, bool is_loop_):
            timeout(ms_),
            dest_tm(dest_ms_),
            callback(t_),
            is_loop(is_loop_)
        {}
        bool is_timeout(uint64_t cur_ms_)       { return dest_tm <= cur_ms_; }
        uint64_t    timeout;
        uint64_t    dest_tm;
        task_t  callback;
        bool    is_loop;
    };
    typedef list<registered_info_t>             registered_info_list_t;
    typedef multimap<long, registered_info_t>   registered_info_map_t;
public:
    timer_service_t(long tick = 1):
        m_runing(true),
        m_efd(-1),
        m_min_timeout(tick),
        m_cache_list(0),
        m_checking_list(1)
    {
        m_efd = ::epoll_create(16);
        
        struct lambda_t
        {
            static void run(void* p_)
            {
                ((timer_service_t*)p_)->run();
            }
        };
        m_thread.create_thread(task_t(&lambda_t::run, this), 1);
    }
    virtual ~timer_service_t()
    {
        m_runing = false;
        interupt();
        
        ::close(m_efd);
        m_thread.join();
    }

    void loop_timer(uint64_t ms_, task_t func)
    {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        uint64_t   dest_ms = tv.tv_sec*1000 + tv.tv_usec / 1000 + ms_;

        lock_guard_t lock(m_mutex);
        m_tmp_register_list.push_back(registered_info_t(ms_, dest_ms, func, true));
    }
    void once_timer(uint64_t ms_, task_t func)
    {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        uint64_t   dest_ms = tv.tv_sec*1000 + tv.tv_usec / 1000 + ms_;
        
        lock_guard_t lock(m_mutex);
        m_tmp_register_list.push_back(registered_info_t(ms_, dest_ms, func, false));
    }
    void timer_callback(uint64_t ms_, task_t func)
    {
        once_timer(ms_, func);
    }

    void run()
    {
        struct epoll_event ev_set[64];
        //! interupt();
        
        struct timeval tv;
        
        do
        {
            ::epoll_wait(m_efd, ev_set, 64, m_min_timeout);
            
            if (false == m_runing)//! cancel
            {
                break;
            }
            
            gettimeofday(&tv, NULL);
            uint64_t cur_ms = tv.tv_sec*1000 + tv.tv_usec / 1000;
            
            add_new_timer();
            process_timer_callback(cur_ms);
            
        }while (true) ;
    }

private:
    void add_new_timer()
    {
        lock_guard_t lock(m_mutex);
        registered_info_list_t::iterator it = m_tmp_register_list.begin();
        for (; it != m_tmp_register_list.end(); ++it)
        {
            m_registered_store.insert(make_pair(it->dest_tm, *it));
        }
        m_tmp_register_list.clear();
    }

    void interupt()
    {
        epoll_event ev = { 0, { 0 } };
        ev.events = EPOLLIN | EPOLLPRI | EPOLLOUT | EPOLLHUP;
        
        ::epoll_ctl(m_efd, EPOLL_CTL_ADD, m_interupt_info.read_fd(), &ev);
    }
    void process_timer_callback(uint64_t now_)
    {
        registered_info_map_t::iterator it_begin = m_registered_store.begin();
        registered_info_map_t::iterator it       = it_begin;

        for (; it != m_registered_store.end(); ++it)
        {
            registered_info_t& last = it->second;
            if (false == last.is_timeout(now_))
            {
                break;
            }
            last.callback.run();
            if (last.is_loop)//! 如果是循环定时器，还要重新加入到定时器队列中
            {
                loop_timer(last.timeout, last.callback);
            }
        }
        
        if (it != it_begin)//! some timeout 
        {
            m_registered_store.erase(it_begin, it);
        }
    }

private:
    volatile bool            m_runing;
    int                      m_efd;
    volatile long            m_min_timeout;
    int                      m_cache_list;
    int                      m_checking_list;
    registered_info_list_t   m_tmp_register_list;
    registered_info_map_t    m_registered_store;
    interupt_info_t          m_interupt_info;
    thread_t                 m_thread;
    mutex_t                  m_mutex;
};

}

#endif

