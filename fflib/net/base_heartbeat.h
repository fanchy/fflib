#ifndef _BASE_HEARTBEAT_H_
#define _BASE_HEARTBEAT_H_

#include <stdint.h>
#include <stdlib.h>

#include <map>
#include <list>
using namespace std;

#include "base/lock.h"
#include "base/arg_helper.h"

namespace ff {

template <typename T>
class base_heartbeat_t
{
    typedef T type_t;
    struct node_info_t
    {
        node_info_t():
            timeout(0)
        {}
        type_t  value;
        time_t  timeout;
    };
    typedef list<node_info_t>                           soft_timeout_list_t;
    typedef map<type_t, typename soft_timeout_list_t::iterator>  data_map_t;
    typedef void (*timeout_callback_t)(type_t);
public:
    base_heartbeat_t();
    ~base_heartbeat_t();
    
    int set_option(const string& opt_, timeout_callback_t fn_);
    int set_option(const arg_helper_t& arg_helper_, timeout_callback_t fn_);

    int add(const type_t& v_);
    int del(const type_t& v_);
    int update(const type_t& v_);

    int timer_check();

    time_t tick() const { return m_cur_tick; }
    
    int timeout() const { return (int)m_timeout; }
private:
    mutex_t                     m_mutex;
    time_t                      m_timeout;
    time_t                      m_cur_tick;
    data_map_t                  m_data_map;
    soft_timeout_list_t         m_time_sort_list;
    timeout_callback_t          m_timeout_callback;
};

template <typename T>
base_heartbeat_t<T>::base_heartbeat_t():
    m_timeout(2147483647 - ::time(NULL)),//! love you 1w year
    m_timeout_callback(NULL)
{
    m_cur_tick = ::time(NULL);
}

template <typename T>
base_heartbeat_t<T>::~base_heartbeat_t()
{
    
}

template <typename T>
int base_heartbeat_t<T>::set_option(const string& opt_, timeout_callback_t fn_)
{
    arg_helper_t arg_helper(opt_);

    return this->set_option(arg_helper, fn_);
}

template <typename T>
int base_heartbeat_t<T>::set_option(const arg_helper_t& arg_helper, timeout_callback_t fn_)
{
    lock_guard_t lock(m_mutex);
    if (arg_helper.is_enable_option("-timeout"))
    {
        m_timeout = time_t(atoi(arg_helper.get_option_value("-timeout").c_str()));
    }
    
    m_timeout_callback = fn_;
    return 0;
}

template <typename T>
int base_heartbeat_t<T>::add(const type_t& v_)
{
    node_info_t info;
    info.value   = v_;
    info.timeout = ::time(NULL) + m_timeout;

    lock_guard_t lock(m_mutex);

    m_time_sort_list.push_front(info);

    pair<typename data_map_t::iterator, bool> ret =  m_data_map.insert(make_pair(v_, m_time_sort_list.begin()));

    if (ret.second == false)
    {
        m_time_sort_list.pop_front();
        return -1;
    }

    return 0;
}

template <typename T>
int base_heartbeat_t<T>::update(const type_t& v_)
{
    time_t new_tm = ::time(NULL) + m_timeout;

    lock_guard_t lock(m_mutex);

    typename data_map_t::iterator it = m_data_map.find(v_);
    if (m_data_map.end() == it)
    {
        return -1;
    }

    node_info_t tmp_info = *(it->second);
    tmp_info.timeout     = new_tm;
    m_time_sort_list.erase(it->second);

    m_time_sort_list.push_front(tmp_info);
    it->second = m_time_sort_list.begin();

    return 0;
}

template <typename T>
int base_heartbeat_t<T>::del(const type_t& v_)
{
    lock_guard_t lock(m_mutex);

    typename data_map_t::iterator it = m_data_map.find(v_);
    if (m_data_map.end() == it)
    {
        return -1;
    }

    m_time_sort_list.erase(it->second);
    m_data_map.erase(it);
    
    return 0;
}

template <typename T>
int base_heartbeat_t<T>::timer_check()
{
    m_cur_tick = ::time(NULL);
    lock_guard_t lock(m_mutex);

    while (false == m_time_sort_list.empty())
    {
        node_info_t& info = m_time_sort_list.back();
        if (m_cur_tick >= info.timeout)
        {
            (*m_timeout_callback)(info.value);
            m_time_sort_list.pop_back();
        }
        else
        {
            break;
        }
    }

    return 0;
}

};
#endif
