#ifndef _LUA_TRAITS_H_
#define _LUA_TRAITS_H_

#include <stdint.h>
#include <stdlib.h>
#include <lua.hpp>

#include <iostream>
using namespace std;


#include "lua/lua_type.h"

//! this traits avoid G++ warnging
//! for example, ARG1 arg1;
//! when type of ARG1 is void *, gcc will print warnging info.
//! so we use ARG1 arg1 = init_value_traits_t<ARG1>::value();

template<typename ARG_TYPE>
struct arg_traits_t;

template<typename ARG_TYPE>
struct arg_traits_t
{
    typedef ARG_TYPE arg_type_t;
};

template<>
struct arg_traits_t<const string&>
{
    typedef string arg_type_t;
};

template<>
struct arg_traits_t<string&>
{
    typedef string arg_type_t;
};

template<typename T>
struct arg_traits_t<const T*>
{
    typedef T* arg_type_t;
};

template <typename T>
struct init_value_traits_t;

template <typename T>
struct init_value_traits_t
{
    inline static T value(){ return 0; }
};

template <typename T>
struct init_value_traits_t<const T*>
{
    inline static T* value(){ return NULL; }
};

template <>
struct init_value_traits_t<string>
{
    inline static const char* value(){ return ""; }
};

template <>
struct init_value_traits_t<const string&>
{
    inline static const char* value(){ return ""; }
};

class lua_object_t;
template <>
struct init_value_traits_t<lua_object_t>
{
    inline static lua_State* value(){ return NULL; }
};

template <>
struct init_value_traits_t<pair<char*, size_t> >
{
    inline static pair<char*, size_t> value(){ return pair<char*, size_t>(NULL, 0); }
};

template <>
struct init_value_traits_t<pair<char*, int> >
{
    inline static pair<char*, int> value(){ return pair<char*, int>(NULL, 0); }
};

template <>
struct init_value_traits_t<pair<const char*, size_t> >
{
    inline static pair<const char*, size_t> value(){ return pair<const char*, size_t>(NULL, 0); }
};

template <>
struct init_value_traits_t<pair<const char*, int> >
{
    inline static pair<const char*, int> value(){ return pair<const char*, int>(NULL, 0); }
};


template<typename T>
class lua_traits_t;

#define LUA_TRAITS_NUM_GEN(X) template<> class lua_traits_t<X> \
    {\
    public:\
        static void push_stack(lua_State* ls_, X arg_)\
        {\
            lua_pushnumber(ls_, (lua_Number)arg_);\
        }\
        \
        static int check_param(lua_State* ls_, int nres_, X& param_)\
        {\
            if (!lua_isnumber(ls_, nres_))\
            {\
                return -1;\
            }\
            \
            param_ = (X)lua_tonumber(ls_, nres_);\
            return 0;\
        }\
        static int check_lua_param(lua_State* ls_, int nres_, X& param_)\
        {\
            param_ = (X)luaL_checknumber(ls_, nres_);\
            return 0;\
        }\
    }

#define LUA_TRAITS_UINT64_NUM_GEN(X) template<> class lua_traits_t<X>    \
{                                                                       \
public:                                                                 \
    static void push_stack(lua_State* ls_, X arg_)                      \
    {                                                                   \
        char buff[21];                                                  \
        int n = sprintf(buff, "%lu", (unsigned long)arg_);                   \
        lua_pushlstring(ls_, buff, n);                                  \
    }                                                                   \
                                                                        \
    static int check_param(lua_State* ls_, int nres_, X& param_)        \
    {                                                                   \
        if (!lua_isstring(ls_, nres_))                                  \
        {                                                               \
            return -1;                                                  \
        }                                                               \
                                                                        \
        size_t len  = 0;                                                \
        const char* src = lua_tolstring(ls_, nres_, &len);              \
        param_ = (X)strtoul(src, NULL, 10);                             \
        return 0;                                                       \
    }                                                                   \
                                                                        \
    static int check_lua_param(lua_State* ls_, int nres_, X& param_)    \
    {                                                                   \
        size_t len = 0;                                                 \
        const char* str = luaL_checklstring(ls_, nres_, &len);          \
        param_ = (X)strtoul(str, NULL, 10);                             \
        return 0;                                                       \
    }                                                                   \
};
#define LUA_TRAITS_INT64_NUM_GEN(X) template<> class lua_traits_t<X>    \
{                                                                       \
public:                                                                 \
    static void push_stack(lua_State* ls_, X arg_)                      \
    {                                                                   \
        char buff[21];                                                  \
        int n = sprintf(buff, "%ld", (unsigned long)arg_);                   \
        lua_pushlstring(ls_, buff, n);                                  \
    }                                                                   \
                                                                        \
    static int check_param(lua_State* ls_, int nres_, X& param_)        \
    {                                                                   \
        if (!lua_isstring(ls_, nres_))                                  \
        {                                                               \
            return -1;                                                  \
        }                                                               \
                                                                        \
        size_t len  = 0;                                                \
        const char* src = lua_tolstring(ls_, nres_, &len);              \
        param_ = (X)strtol(src, NULL, 10);                             \
        return 0;                                                       \
    }                                                                   \
                                                                        \
    static int check_lua_param(lua_State* ls_, int nres_, X& param_)    \
    {                                                                   \
        size_t len = 0;                                                 \
        const char* str = luaL_checklstring(ls_, nres_, &len);          \
        param_ = (X)strtol(str, NULL, 10);                             \
        return 0;                                                       \
    }                                                                   \
};

template<>
class lua_traits_t<void*>
{
public:
    static void push_stack(lua_State* ls_, void* arg_)
    {
        lua_pushlightuserdata(ls_, arg_);
    }

    static int check_param(lua_State* ls_, int nres_, void* & param_)
    {
        if (!lua_isuserdata(ls_, nres_))
        {
            char buff[512];
            snprintf(buff, sizeof(buff), "userdata param expected, but type<%s> provided",
                                         lua_typename(ls_, lua_type(ls_, nres_)));
            cout << buff << "\n";
            return -1;
        }

        param_ = lua_touserdata(ls_, nres_);
        return 0;
    }

    static int check_lua_param(lua_State* ls_, int nres_, void*& param_)
    {
        if (!lua_isuserdata(ls_, nres_))
        {
            luaL_argerror (ls_, 1, "userdata param expected");
            return -1;
        }
        param_ = lua_touserdata(ls_, nres_);
        return 0;
    }
};

template<typename T>
class lua_traits_t<T*>
{
public:
    static void push_stack(lua_State* ls_, T* arg_)
    {
        void* ptr = lua_newuserdata(ls_, sizeof(class_userdata_t<T>));
        new (ptr) class_userdata_t<T>(arg_);

        luaL_getmetatable(ls_, lua_type_registry_t<T>::get_type_name());
        lua_setmetatable(ls_, -2);
    }

    static int check_param(lua_State* ls_, int nres_, T* & param_)
    {
        if (false == lua_type_registry_t<T>::is_registed())
        {
            luaL_argerror(ls_, nres_, "arg can't convert to class*, because the class has not registed to Lua");
        }

        //! void* arg_data = luaL_checkudata(ls_, nres_, lua_type_registry_t<T>::get_type_name());
        void *arg_data = lua_touserdata(ls_, nres_);

        if (NULL == arg_data)
        {
            char buff[512];
            snprintf(buff, sizeof(buff), "userdata<%s> param expected, but type<%s> provided",
                                         lua_type_registry_t<T>::get_type_name(),
                                         lua_typename(ls_, lua_type(ls_, nres_)));
            cout << buff << "\n";
            return -1;
        }

        if (0 == lua_getmetatable(ls_, nres_))
        {  //! does it have a metatable?
            return -1;
        }

        //! get correct metatable
        lua_getfield(ls_, LUA_REGISTRYINDEX, lua_type_registry_t<T>::get_type_name());
        if (0 == lua_rawequal(ls_, -1, -2))
        {//! does it have the correct mt?
            char buff[512];
            snprintf(buff, sizeof(buff), "userdata<%s> param expected, but type<%s> provided",
                                         lua_type_registry_t<T>::get_type_name(),
                                         lua_typename(ls_, lua_type(ls_, nres_)));
            cout << buff << "\n";
            lua_pop(ls_, 2);
            return -1;
        }

        //1 remove both metatables
        lua_pop(ls_, 2);

        T* ret_ptr = ((class_userdata_t<T>*)arg_data)->obj_ptr;
        if (NULL == ret_ptr)
        {
            return -1;
        }

        param_ = ret_ptr;
        return 0;
    }

    static int check_lua_param(lua_State* ls_, int nres_, T*& param_)
    {
        if (false == lua_type_registry_t<T>::is_registed())
        {
            luaL_argerror(ls_, nres_, "arg can't convert to class*, because the class has not registed to Lua");
        }

        void* arg_data = luaL_checkudata(ls_, nres_, lua_type_registry_t<T>::get_type_name());

        if (NULL == arg_data)
        {
            char buff[512];
            snprintf(buff, sizeof(buff), "`%s` expect arg 1, but arg == null",
                                         lua_type_registry_t<T>::get_type_name());
            luaL_argerror(ls_, nres_, buff);
        }

        T* ret_ptr = ((class_userdata_t<T>*)arg_data)->obj_ptr;
        if (NULL == ret_ptr)
        {
            char buff[512];
            snprintf(buff, sizeof(buff), "`%s` object ptr can't be null",
                                         lua_type_registry_t<T>::get_type_name());
            luaL_argerror(ls_, nres_, buff);
        }

        param_ = ret_ptr;
        return 0;
    }
};

template<typename T>
class lua_traits_t<const T*>
{
public:
    static void push_stack(lua_State* ls_, const T* arg_)
    {
        lua_traits_t<T*>::push_stack(ls_, (T*)arg_);
    }

    static int check_param(lua_State* ls_, int nres_, T* & param_)
    {
       return lua_traits_t<T*>::check_param(ls_, nres_, param_);
    }

    static int check_lua_param(lua_State* ls_, int nres_, T*& param_)
    {
        return lua_traits_t<T*>::check_lua_param(ls_, nres_, param_);
    }
};
LUA_TRAITS_NUM_GEN(int8_t);
LUA_TRAITS_NUM_GEN(uint8_t);
LUA_TRAITS_NUM_GEN(int16_t);
LUA_TRAITS_NUM_GEN(uint16_t);
LUA_TRAITS_NUM_GEN(int32_t);
LUA_TRAITS_NUM_GEN(uint32_t);
LUA_TRAITS_NUM_GEN(float);
LUA_TRAITS_NUM_GEN(double);
LUA_TRAITS_INT64_NUM_GEN(uint64_t);
LUA_TRAITS_UINT64_NUM_GEN(int64_t);
LUA_TRAITS_INT64_NUM_GEN(long);


template<>
class lua_traits_t<bool>
{
public:
    static void push_stack(lua_State* ls_, bool arg_)
    {
        lua_pushboolean(ls_, arg_);
    }

    static int check_param(lua_State* ls_, int nres_, bool& param_)
    {
    	//! nil 自动转换为false
    	if (lua_isnil(ls_, nres_))
    	{
    		param_ = false;
    		return 0;
    	}
        if (!lua_isboolean(ls_, nres_))
        {
            return -1;
        }

        param_ = (bool)lua_toboolean(ls_, nres_);
        return 0;
    }
    static int check_lua_param(lua_State* ls_, int nres_, bool& param_)
    {
		luaL_checktype(ls_, nres_,  LUA_TBOOLEAN);
        param_ = (bool)lua_toboolean(ls_, nres_);
        return 0;
    }
};

template<>
class lua_traits_t<string>
{
public:
    static void push_stack(lua_State* ls_, const string& arg_)
    {
        lua_pushlstring(ls_, arg_.c_str(), arg_.length());
    }

    static int check_param(lua_State* ls_, int nres_, string& param_)
    {
        if (!lua_isstring(ls_, nres_))
        {
            return -1;
        }

        //! what's amazing!  lua_tolstring also changes the actual value in the stack to a string
        //! This change confuses lua_next when lua_tolstring is applied to keys during a table traversal.
        lua_pushvalue(ls_, nres_);
        size_t len  = 0;
        const char* src = lua_tolstring(ls_, -1, &len);
        param_.assign(src, len);
        lua_pop(ls_, 1);

        return 0;
    }
    static int check_lua_param(lua_State* ls_, int nres_, string& param_)
    {
        size_t len = 0;
        const char* str = luaL_checklstring(ls_, nres_, &len);
        param_.assign(str, len);
        return 0;
    }
};

template<>
class lua_traits_t<const string&>
{
public:
    static void push_stack(lua_State* ls_, const string& arg_)
    {
        lua_pushlstring(ls_, arg_.c_str(), arg_.length());
    }

    static int check_param(lua_State* ls_, int nres_, string& param_)
    {
        if (!lua_isstring(ls_, nres_))
        {
            return -1;
        }

        //! what's amazing!  lua_tolstring also changes the actual value in the stack to a string
        //! This change confuses lua_next when lua_tolstring is applied to keys during a table traversal.
        lua_pushvalue(ls_, nres_);
        size_t len  = 0;
        const char* src = lua_tolstring(ls_, -1, &len);
        param_.assign(src, len);
        lua_pop(ls_, 1);

        return 0;
    }
    static int check_lua_param(lua_State* ls_, int nres_, string& param_)
    {
        size_t len = 0;
        const char* str = luaL_checklstring(ls_, nres_, &len);
        param_.assign(str, len);
        return 0;
    }
};

template<>
class lua_traits_t<const char*>
{
public:
    static void push_stack(lua_State* ls_, const char* arg_)
    {
        lua_pushstring(ls_, arg_);
    }
};

template<>
class lua_traits_t<lua_nil_t>
{
public:
    static void push_stack(lua_State* ls_, const lua_nil_t& arg_)
    {
        lua_pushnil (ls_);
    }

};

template<>
class lua_traits_t<pair<const char*, size_t> >
{
public:
    static void push_stack(lua_State* ls_, const pair<const char*, size_t>& arg_)
    {
        lua_pushlstring(ls_, arg_.first, arg_.second);
    }

    static int check_lua_param(lua_State* ls_, int nres_, pair<const char*, size_t>& param_)
    {
        size_t len = 0;
        param_.first  = luaL_checklstring(ls_, nres_, &len);
        param_.second = len;
        return 0;
    }
};

template<>
class lua_traits_t<pair<const char*, int> >
{
public:
    static void push_stack(lua_State* ls_, const pair<const char*, int>& arg_)
    {
        lua_pushlstring(ls_, arg_.first, arg_.second);
    }

    static int check_lua_param(lua_State* ls_, int nres_, pair<const char*, int>& param_)
    {
        size_t len = 0;
        param_.first  = luaL_checklstring(ls_, nres_, &len);
        param_.second = (int)len;
        return 0;
    }
};


class lua_object_t;
template<>
class lua_traits_t<lua_object_t>
{
public:

    template<typename T>
    static void push_stack(lua_State* ls_, const T& arg_)
    {
        lua_pushvalue(ls_, arg_.stack_index());
    }

    template<typename T>
    static int check_param(lua_State* ls_, int nres_, T& param_)
    {
        param_.init(ls_, lua_gettop(ls_), true);//! auto relese
        return 0;
    }

    template<typename T>
    static int check_lua_param(lua_State* ls_, int nres_, T& param_)
    {
        param_.init(ls_, nres_, false);//! no auto relese
        return 0;
    }

};


#endif
