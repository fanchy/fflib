#ifndef _LUA_OBJECT_H_
#define _LUA_OBJECT_H_

#include <lua.hpp>
#include <string>
using namespace std;

#include "lua_type.h"

class lua_object_t
{
public:
    lua_object_t();
    lua_object_t(lua_State* ls_);

    lua_object_t& operator=(const lua_object_t& obj_);

    virtual ~lua_object_t();
    virtual void init(lua_State* ls_, int nres_, bool auto_pop_flag_);
    virtual void release();
    lua_State* get_lua_state() const;
    int  stack_index() const;
    bool get_flag() const;

    int  type() const;
    string type_name() const;

    bool is_boolean() const;
    bool is_cfunction() const;
    bool is_function() const;
    bool is_lightuserdata() const;
    bool is_nil() const;
    bool is_none() const;
    bool is_noneornil() const;
    bool is_number() const;
    bool is_string() const;
    bool is_table() const;
    bool is_thread() const;
    bool is_userdata() const;

    bool to_boolean() const;
    lua_function_t to_cfunction() const;
    int to_integer() const;
    double to_number() const;
    const void* to_pointer();
    string to_string() const;
    lua_State* to_thread() const;
    void* to_userdata() const;
private:
    lua_State*  m_ls;
    int         m_stack_index;
    bool        m_auto_pop_flag;
};

#endif
