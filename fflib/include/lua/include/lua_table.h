#ifndef _LUA_TABLE_H_
#define _LUA_TABLE_H_

#include <lua.hpp>
#include <boost/shared_ptr.hpp>
#include "detail/lua_traits.h"

class lua_table_t
{
public:
    lua_table_t();
    lua_table_t(lua_State* ls_, int nres_ = -1, bool auto_pop_flag_= true);
    virtual ~lua_table_t();

    void init(lua_State* ls_, int nres_, bool auto_pop_flag_);
    bool end();
    bool next();
    void begin();
    int  table_index() const;
    void release();

    template<typename T, typename R>
    int get_pair(T& key_, R& ret_);
    template<typename T, typename R>
    int get_pair(pair<T, R>& ret_);
    template<typename T>
    int get_key(T& key_);
    template<typename T>
    int get_value(T& ret_);
    int get_value(lua_table_t& ret_);

    template<typename T>
    int find(int key_, T& ret_);
    template<typename T>
    int find(const string& key_, T& ret_);

    int set(int key_, const char* value_){return set<string>(key_, string(value_));}
    int set(const string& key_, const char* value_){return set<string>(key_, string(value_));}

    template<typename T>
    int set(int key_, const T& value_);
    template<typename T>
    int set(const string& key_, const T& value_);

    template<typename T>
    int del(T& key_);

private:
    void locat_begin_i();
private:
    lua_State*  m_ls;
    int         m_table_index;
    bool        m_is_end;
    bool        m_auto_pop_flag;
};

class auto_lua_table_t: public lua_table_t
{
    typedef lua_table_t _super_t;
public:
    auto_lua_table_t();
    ~auto_lua_table_t();
};

typedef boost::shared_ptr<auto_lua_table_t> auto_lua_table_ptr_t;

template<typename T, typename R>
int lua_table_t::get_pair(T& key_, R& ret_)
{
    if (0 == get_key(key_))
    {
        return get_value(ret_);
    }

    return -1;
}

template<typename KEY, typename VALUE>
int lua_table_t::get_pair(pair<KEY, VALUE>& ret_)
{
    if (0 == get_key(ret_.first))
    {
        return get_value(ret_.second);
    }

    return -1;
}

template<typename KEY>
int lua_table_t::get_key(KEY& ret_)
{
    return lua_traits_t<KEY>::check_param(m_ls, -2, ret_);
}

template<typename VALUE>
int lua_table_t::get_value(VALUE& ret_)
{
    return lua_traits_t<VALUE>::check_param(m_ls, -1, ret_);
}

template<typename T>
int lua_table_t::find(int key_, T& ret_)
{
    int ret_flag = 0;

    lua_pushnumber(m_ls, key_);
    lua_gettable(m_ls, m_table_index);

    ret_flag = lua_traits_t<T>::check_param(m_ls, -1, ret_);
    lua_pop(m_ls, 1);
    return ret_flag;
}

template<typename T>
int lua_table_t::find(const string& key_, T& ret_)
{
    int ret_flag = 0;

    lua_pushlstring(m_ls, key_.c_str(), key_.length());
    lua_gettable(m_ls, m_table_index);

    ret_flag = lua_traits_t<T>::check_param(m_ls, -1, ret_);
    lua_pop(m_ls, 1);
    return ret_flag;
}

template<typename T>
int lua_table_t::set(int key_, const T& value_)
{
    lua_traits_t<int>::push_stack(m_ls, key_);
    lua_traits_t<T>::push_stack(m_ls, value_);

    lua_settable(m_ls, m_table_index);
    return 0;
}

template<typename T>
int lua_table_t::set(const string& key_, const T& value_)
{
    lua_traits_t<string>::push_stack(m_ls, key_);
    lua_traits_t<T>::push_stack(m_ls, value_);

    lua_settable(m_ls, m_table_index);
    return 0;
}

template<typename T>
int lua_table_t::del(T& key_)
{
    return set(key_, lua_nil_t());
}

#endif
