
#ifndef _FF_BUS_I_
#define _FF_BUS_I_

#include <map>
#include <vector>
#include <iostream>
using namespace std;

#include "cqrs/event_i.h"
#include "cqrs/command_i.h"

namespace ff
{

class subscriber_cb_t
{
public:
    virtual ~subscriber_cb_t(){}
    virtual void callback(const type_i* obj_i) = 0;
    virtual void callback(const string& data_) = 0;
    virtual subscriber_cb_t* fork() const = 0;
};

template<typename T, typename ARG>
class subscriber_cb_impl_t: public subscriber_cb_t
{
    typedef void (T::*dest_func_t)(const ARG&); 
public:
    virtual ~subscriber_cb_impl_t(){}
    subscriber_cb_impl_t(T* obj_, dest_func_t func_):
        m_obj(obj_),
        m_func(func_)
    {}
    
    virtual void callback(const type_i* obj_)
    {
         (m_obj->*m_func)(*((ARG*)obj_));
    }
    virtual void callback(const string& data_)
    {
        ARG obj;
        obj.decode(data_);
        (m_obj->*m_func)((obj));
    }
    virtual subscriber_cb_t* fork() const
    {
        return new subscriber_cb_impl_t<T, ARG>(m_obj, m_func);
    }
    T* m_obj;
    dest_func_t m_func;
};

template<typename T, typename ARG>
class subscriber_cb_for_handle_t: public subscriber_cb_t
{
    typedef void (T::*dest_func_t)(const ARG&); 
public:
    virtual ~subscriber_cb_for_handle_t(){}
    subscriber_cb_for_handle_t(T* obj_):
        m_obj(obj_)
    {}
    
    virtual void callback(const type_i* obj_)
    {
         (m_obj->handle)(*((ARG*)obj_));
    }
    virtual void callback(const string& data_)
    {
        ARG obj;
        obj.decode(data_);
        (m_obj->handle)((obj));
    }
    virtual subscriber_cb_t* fork() const
    {
        return new subscriber_cb_for_handle_t<T, ARG>(m_obj);
    }
    T* m_obj;
};

class subscriber_t
{
public:
    typedef map<int, subscriber_cb_t*>          callback_map_t;
    typedef multimap<int, subscriber_cb_t*>     callback_multimap_t;
public:
    subscriber_t(){}
    subscriber_t(const subscriber_t& src_)
    {
        for (callback_map_t::const_iterator it = src_.m_callbacks.begin(); it != src_.m_callbacks.end(); ++it)
        {
            m_callbacks.insert(make_pair(it->first, it->second-fork()));
        }
    }
    subscriber_t& operator=(const subscriber_t& src_)
    {
        release();
        for (callback_map_t::const_iterator it = src_.m_callbacks.begin(); it != src_.m_callbacks.end(); ++it)
        {
            m_callbacks.insert(make_pair(it->first, it->second-fork()));
        }
        return *this;
    }
    virtual ~subscriber_t()
    {
        release();
    }
    void release()
    {
        for (callback_map_t::iterator it = m_callbacks.begin(); it != m_callbacks.end(); ++it)
        {
            delete it->second;
        }
        m_callbacks.clear();
    }

    template<typename T, typename ARG>
    subscriber_t& reg(T* obj_, void (T::*dest_func)(const ARG&))
    {
        m_callbacks.insert(make_pair(TYPEID(ARG), new subscriber_cb_impl_t<T, ARG>(obj_, dest_func)));
        return *this;
    }
    template<typename ARG, typename T>
    subscriber_t& reg(T* obj_)
    {
        m_callbacks.insert(make_pair(TYPEID(ARG), new subscriber_cb_for_handle_t<T, ARG>(obj_)));
        return *this;
    }

    callback_map_t& get_callback_map() { return m_callbacks; }
protected:
    callback_map_t m_callbacks;
};

class bus_i
{
public:
    virtual ~bus_i(){}

    virtual int publish(const event_i& event_)  = 0;
    virtual int publish(const command_i& cmd_)  = 0;
    virtual void subscribe(subscriber_t&)       = 0;
};

class bus_t
{
public:
    virtual ~bus_t()
    {
        for (subscriber_t::callback_multimap_t::iterator it = m_callbacks.begin(); it != m_callbacks.end(); ++it)
        {
            delete it->second;
        }
        m_callbacks.clear();
    }
    virtual int publish(const event_i& event_)
    {
        return call(event_.get_type_id(), event_);
    }
    virtual int publish(const command_i& cmd_)
    {
        return call(cmd_.get_type_id(), cmd_);
    }
    virtual int publish(const string& type_name_, const string& data_)
    {
        int type_id = TYPE_NAME_TO_ID(type_name_);
        return call(type_id, data_);
    }
    virtual void subscribe(subscriber_t& sub_)
    {
        subscriber_t::callback_map_t::iterator it = sub_.get_callback_map().begin();
        for (; it != sub_.get_callback_map().end(); ++it)
        {
            m_callbacks.insert(make_pair(it->first, it->second->fork()));
        }               
    }

protected:

    int call(int type_id_, const type_i& obj_)
    {
        int num = 0;
        pair<subscriber_t::callback_multimap_t::iterator, subscriber_t::callback_multimap_t::iterator> ret;
        ret = m_callbacks.equal_range(type_id_);

        for (subscriber_t::callback_multimap_t::iterator it = ret.first; it != ret.second; ++it)
        {
            try
            {
                ++num;
                it->second->callback(&obj_);
            }
            catch(exception& e)
            {
                cout <<"bus exception:" << e.what() <<"\n";
                continue;
            }
            return 0;
        }
        return num;
    }
    
    int call(int type_id_, const string& data_)
    {
        int num = 0;
        pair<subscriber_t::callback_multimap_t::iterator, subscriber_t::callback_multimap_t::iterator> ret;
        ret = m_callbacks.equal_range(type_id_);

        for (subscriber_t::callback_multimap_t::iterator it = ret.first; it != ret.second; ++it)
        {
            try
            {
                ++num;
                it->second->callback(data_);
            }
            catch(exception& e)
            {
                cout <<"bus exception:" << e.what() <<"\n";
                continue;
            }
            return 0;
        }
        return num;
    }
private:
    subscriber_t::callback_multimap_t        m_callbacks;
};

#define BUS  singleton_t<bus_t>::instance()
}
#endif
