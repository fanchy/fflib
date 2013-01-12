
#ifndef _FF_TYPE_I_
#define _FF_TYPE_I_

#include "utility/singleton.h"
#include "lock.h"
#include <string>
#include <map>
using namespace std;

namespace ff
{
    
#define TYPEID(X)              singleton_t<type_helper_t<X> >::instance().id()
#define TYPE_NAME(X)           singleton_t<type_helper_t<X> >::instance().get_type_name()
#define TYPE_NAME_TO_ID(name_) singleton_t<type_id_generator_t>::instance().get_id_by_name(name_)

class type_i
{
public:
    virtual ~ type_i(){}
    virtual int get_type_id() const { return -1; }
    virtual const string& get_type_name() const {static string foo; return foo; }
    
    virtual void   decode(const string& data_) {}
    virtual string encode()                    { return "";} 
};

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

}
#endif
