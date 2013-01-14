#ifndef _FM_LUA_JSON_CONVERTER_H_
#define _FM_LUA_JSON_CONVERTER_H_//! 类型定义

#include <stdint.h>

#include <vector>
#include <string>
using namespace std;

#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "lua.hpp"
#include "fm_json.h"

class fm_lua_json_converter_t
{
    typedef boost::function<void (lua_State* ls_)>  push_json_function_t;
public:
    fm_lua_json_converter_t();
    ~fm_lua_json_converter_t();

    int decode(const string& json_);
    int to_lua_table(lua_State* ls_);

private:
    void process_object(Json::Value& json_val_);
    void process_array(Json::Value& json_val_);

private:
    static void lua_create_table(lua_State* ls_)
    {
        lua_newtable(ls_);
    }

    static void lua_set_table_int_int(lua_State* ls_, int key_, int64_t val_)
    {
        lua_pushnumber(ls_, key_);
        lua_pushnumber(ls_, val_);
        lua_settable(ls_, -3);
    }
    static void lua_set_table_int_uint(lua_State* ls_, int key_, uint64_t val_)
    {
        lua_pushnumber(ls_, key_);
        lua_pushnumber(ls_, val_);
        lua_settable(ls_, -3);
    }
    static void lua_set_table_int_double(lua_State* ls_, int key_, double val_)
    {
        lua_pushnumber(ls_, key_);
        lua_pushnumber(ls_, val_);
        lua_settable(ls_, -3);
    }
    static void lua_set_table_int_string(lua_State* ls_, int key_, string val_)
    {
        lua_pushnumber(ls_, key_);
        lua_pushlstring(ls_, val_.c_str(), val_.length());
        lua_settable(ls_, -3);
    }
    static void lua_set_table_int_bool(lua_State* ls_, int key_, bool val_)
    {
        lua_pushnumber(ls_, key_);
        lua_pushboolean(ls_, val_);
        lua_settable(ls_, -3);
    }

    //! --------------------------------------------------------------------------
    static void lua_set_table_string_int(lua_State* ls_, string key_, int64_t val_)
    {
        lua_pushlstring(ls_, key_.c_str(), key_.length());
        lua_pushnumber(ls_, val_);
        lua_settable(ls_, -3);
    }
    static void lua_set_table_string_uint(lua_State* ls_, string key_, uint64_t val_)
    {
        lua_pushlstring(ls_, key_.c_str(), key_.length());
        lua_pushnumber(ls_, val_);
        lua_settable(ls_, -3);
    }
    static void lua_set_table_string_double(lua_State* ls_, string key_, double val_)
    {
        lua_pushlstring(ls_, key_.c_str(), key_.length());
        lua_pushnumber(ls_, val_);
        lua_settable(ls_, -3);
    }
    static void lua_set_table_string_string(lua_State* ls_, string key_, string val_)
    {
        lua_pushlstring(ls_, key_.c_str(), key_.length());
        lua_pushlstring(ls_, val_.c_str(), val_.length());
        lua_settable(ls_, -3);
    }
    static void lua_set_table_string_bool(lua_State* ls_, string key_, bool val_)
    {
        lua_pushlstring(ls_, key_.c_str(), key_.length());
        lua_pushboolean(ls_, val_);
        lua_settable(ls_, -3);
    }

    //! ---------------------------------------------------------------------------
    static void lua_set_table_string_key(lua_State* ls_, string key_)
    {
        lua_pushlstring(ls_, key_.c_str(), key_.length());
    }
    static void lua_set_table_int_key(lua_State* ls_, int key_)
    {
        lua_pushnumber(ls_, key_);
    }
    static void lua_set_table_stack_arg(lua_State* ls_)
    {
        lua_settable(ls_, -3);
    }
private:
    vector<push_json_function_t>    m_func_vt;
};
#endif //FM_LUA_JSON_CONVERTER_H_
