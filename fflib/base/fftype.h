
#ifndef _FF_TYPE_H_
#define _FF_TYPE_H_

#include "base/singleton.h"
#include "base/lock.h"

#include <stdint.h>
#include <list>
#include <fstream>
#include <string>
#include <map>
using namespace std;

namespace ff
{
    
#define TYPEID(X)              singleton_t<type_helper_t<X> >::instance().id()
#define TYPE_NAME(X)           singleton_t<type_helper_t<X> >::instance().get_type_name()
#define TYPE_NAME_TO_ID(name_) singleton_t<type_id_generator_t>::instance().get_id_by_name(name_)

    
struct type_id_generator_t
{
    type_id_generator_t():m_id(0){}
    int alloc_id(const string& name_)
    {
        lock_guard_t lock(m_mutex);
        int id = ++ m_id;
        m_name2id[name_] = id;
        return id;
    }
    int get_id_by_name(const string& name_) const
    {
        map<string, int>::const_iterator it = m_name2id.find(name_);
        if (it != m_name2id.end())
        {
            return it->second;
        }
        return 0;
    }
    mutex_t          m_mutex;
    int              m_id;
    map<string, int> m_name2id;
};

template<typename T>
struct type_helper_t
{
    type_helper_t():
        m_type_id(0)
    {
        string tmp  = __PRETTY_FUNCTION__;
        int pos     = tmp.find("type_helper_t() [with T = ");
        m_type_name = tmp.substr(pos + 26, tmp.size() - 26 - pos - 1);
        m_type_id   = singleton_t<type_id_generator_t>::instance().alloc_id(m_type_name);
    }
    int id() const
    {
        return m_type_id;
    }
    const string& get_type_name() const
    {
        return m_type_name;
    }
    int     m_type_id;
    string  m_type_name;
};

class type_i
{
public:
    virtual ~ type_i(){}
    virtual int get_type_id() const { return -1; }
    virtual const string& get_type_name() const {static string foo; return foo; }
    
    virtual void   decode(const string& data_) {}
    virtual string encode()                    { return "";} 
    
    template<typename T>
    T* cast()
    {
        if (get_type_id() == TYPEID(T))
        {
            return (T*)this;
        }
        return NULL;
    }
};


template<typename SUPERT, typename T>
class auto_type_t: public SUPERT
{
public:
    virtual ~ auto_type_t(){}
    virtual int get_type_id() const
    {
        return TYPEID(T);
    }
    virtual const string& get_type_name() const
    {
        return TYPE_NAME(T);
    }
};

class obj_counter_i
{
public:
    obj_counter_i():m_ref_count(0){}
    virtual ~ obj_counter_i(){}
    void inc(int n) { (void)__sync_add_and_fetch(&m_ref_count, n); }
    void dec(int n) { __sync_sub_and_fetch(&m_ref_count, n); 	   }
    long val() const{ return m_ref_count; 						   }

    virtual const string& get_name() { static string ret; return ret; }
protected:
    volatile long m_ref_count;
};

class obj_summary_t
{
public:
    void reg(obj_counter_i* p)
    {
        lock_guard_t lock(m_mutex);
        m_all_counter.push_back(p);
    }

    map<string, long> get_all_obj_num()
    {
        lock_guard_t lock(m_mutex);
        map<string, long> ret;
        for (list<obj_counter_i*>::iterator it = m_all_counter.begin(); it != m_all_counter.end(); ++it)
        {
            ret.insert(make_pair((*it)->get_name(), (*it)->val()));
        }
        return ret;
    }

    void dump(const string& path_)
    {
        ofstream tmp_fstream;
        tmp_fstream.open(path_.c_str(), ios::app);
        map<string, long> ret = get_all_obj_num();
        map<string, long>::iterator it = ret.begin();

        time_t timep   = time(NULL);
        struct tm *tmp = localtime(&timep);

        char tmp_buff[256];
        sprintf(tmp_buff, "%04d%02d%02d-%02d:%02d:%02d",
                        tmp->tm_year + 1900, tmp->tm_mon + 1, tmp->tm_mday,
                        tmp->tm_hour, tmp->tm_min, tmp->tm_sec);
        char buff[1024] = {0};

        tmp_fstream << "obj,num,time\n";

        for (; it != ret.end(); ++it)
        {
            snprintf(buff, sizeof(buff), "%s,%ld,%s\n", it->first.c_str(), it->second, tmp_buff);
            tmp_fstream << buff;
        }

        tmp_fstream.flush();
    }
protected:
    mutex_t                     m_mutex;
    list<obj_counter_i*>	m_all_counter;
};

template<typename T>
class obj_counter_t: public obj_counter_i
{
public:
    obj_counter_t()
    {
        singleton_t<obj_summary_t>::instance().reg(this);
    }
    virtual const string& get_name() { return TYPE_NAME(T); }
};

template<typename T, typename R = type_i>
class fftype_t: public R
{
public:
    fftype_t()
    {
        singleton_t<obj_counter_t<T> >::instance().inc(1);
    }
    virtual ~ fftype_t()
    {
        singleton_t<obj_counter_t<T> >::instance().dec(1);
    }
    virtual int get_type_id() const
    {
        return TYPEID(T);
    }
    virtual const string& get_type_name() const
    {
        return TYPE_NAME(T);
    }
};

class ffattr_t
{
public:
    typedef uint64_t number_t;

public:
    virtual ~ffattr_t(){}

    number_t get_num(number_t key_)
    {
        map<number_t, number_t>::iterator it = m_num2num.find(key_);
        if (it != m_num2num.end())
        {
            return it->second;
        }
        return 0;
    }
    const string& get_string(number_t key_)
    {
        map<number_t, string>::iterator it = m_num2string.find(key_);
        if (it != m_num2string.end())
        {
            return it->second;
        }
        static string dumy_str;
        return dumy_str;
    }
    number_t get_num(const string& key_)
    {
        map<string, number_t>::iterator it = m_string2num.find(key_);
        if (it != m_string2num.end())
        {
            return it->second;
        }
        return 0;
    }
    const string& get_string(const string& key_)
    {
        map<string, string>::iterator it = m_string2string.find(key_);
        if (it != m_string2string.end())
        {
            return it->second;
        }
        static string dumy_str;
        return dumy_str;
    }

    void set_num(number_t key_, number_t val_)
    {
        m_num2num[key_] = val_;
    }
    void set_string(number_t key_, const string& val_)
    {
        m_num2string[key_] = val_;
    }
    void set_num(const string& key_, number_t val_)
    {
        m_string2num[key_] = val_;
    }
    void set_string(const string& key_, const string& val_)
    {
        m_string2string[key_] = val_;
    }
    
    bool is_exist_num(number_t key_)
    {
        return m_num2num.find(key_) != m_num2num.end();
    }
    bool is_exist_string(number_t key_)
    {
        return m_num2string.find(key_) != m_num2string.end();
    }
    bool is_exist_num(const string& key_)
    {
        return m_string2num.find(key_) != m_string2num.end();
    }
    bool is_exist_string(const string& key_)
    {
        return m_string2string.find(key_) != m_string2string.end();
    }
    
    map<number_t, number_t>& get_num2num()       { return m_num2num;       }
    map<number_t, string>&   get_num2string()    { return m_num2string;    }
    map<string, number_t>&   get_string2num()    { return m_string2num;    }
    map<string, string>&     get_string2string() { return m_string2string; }

private:
    map<number_t, number_t>    m_num2num;
    map<number_t, string>      m_num2string;
    map<string, number_t>      m_string2num;
    map<string, string>        m_string2string;
};
}
#endif
