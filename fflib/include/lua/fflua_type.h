#ifndef _FF_LUA_TYPE_H_
#define _FF_LUA_TYPE_H_

#include <stdint.h>
#include <stdlib.h>
#include <lua.hpp>
#include <string.h>

struct lua_string_tool_t
{
	inline static const char* c_str(const string& s_) { return s_.c_str(); }
	inline static const char* c_str(const char* s_)   { return s_; }
};

class lua_exception_t: public exception
{
public:
    explicit lua_exception_t(const char* err_):
		m_err(err_)
	{}
    explicit lua_exception_t(const string& err_):
		m_err(err_)
	{
	}
    ~lua_exception_t() throw (){}

    const char* what() const throw () { return m_err.c_str(); }
private:
    string m_err;
};

class lua_helper
{
public:
    static void stack_dump(lua_State* ls_)
    {
        int i;
        int top = lua_gettop(ls_);

        for (i = 1; i <= top; i++)
        {
            /* repeat for each level */
            int t = lua_type(ls_, i);
            switch (t)
            {
                case LUA_TSTRING:
                {
                    printf("`%s'", lua_tostring(ls_, i));
                }
                break;
                case LUA_TBOOLEAN: /* booleans */
                {
                    printf(lua_toboolean(ls_, i) ? "true" : "false");
                }
                break;
                case LUA_TNUMBER: /* numbers */
                {
                    printf("`%g`", lua_tonumber(ls_, i));
                }
                break;
                case LUA_TTABLE:
                {
                    /*
                     lua_pushnil(ls_);
                     while (lua_next(ls_, i) != 0) {
                       printf("%s - %s\n",
                              lua_typename(ls_, lua_type(ls_, -2)),
                              lua_typename(ls_, lua_type(ls_, -1)));
                       lua_pop(ls_, 1);
                     }
                     */
                     printf("`%s'", "table");
                }
                break;
                default: /* other values */
                {
                    printf("`%s`", lua_typename(ls_, t));
                }
                break;
            }
            printf(" "); /* put a separator */
        }
        printf("\n"); /* end the listing */
    }
    static string dump_error(lua_State* ls_, const char *fmt, ...)
    {
        string ret;
        char buff[1024];

        va_list argp;
        va_start(argp, fmt);
        vsnprintf(buff, sizeof(buff), fmt, argp);
        va_end(argp);

        ret = buff;
        snprintf(buff, sizeof(buff), " tracback:%s", lua_tostring(ls_, -1));
        ret += buff;

        return ret;
    }
};

typedef int (*lua_function_t) (lua_State *L);

class lua_nil_t{};

template<typename T>
struct class_userdata_t
{
    class_userdata_t():
        obj_ptr(NULL)
    {
    }
    class_userdata_t(T* p_):
        obj_ptr(p_)
    {
    }
public:
    T* obj_ptr;
};


template<typename T>
struct lua_type_info_t
{
	static void set_name(const string& name_)
	{
		size_t n = name_.length() > sizeof(name) - 1? sizeof(name) - 1: name_.length();
		::strncpy(name, name_.c_str(), n);
	}
	static const char* get_name()
	{
		return name;
	}
	static bool is_registed()
	{
		return name[0] != '\0';
	}
	static char name[128];
};
template<typename T>
char lua_type_info_t<T>::name[128] = {0};


typedef int (*index_impl_func_t)(lua_State*, void*, const char*);
typedef int (*newindex_impl_func_t)(lua_State*, void*, const char*, int);

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
            printf("%s\n", buff);
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

        luaL_getmetatable(ls_, lua_type_info_t<T>::get_name());
        lua_setmetatable(ls_, -2);
    }

    static int check_param(lua_State* ls_, int nres_, T* & param_)
    {
        if (false == lua_type_info_t<T>::is_registed())
        {
            luaL_argerror(ls_, nres_, "arg can't convert to class*, because the class has not registed to Lua");
        }

        //! void* arg_data = luaL_checkudata(ls_, nres_, lua_type_info_t<T>::get_name());
        void *arg_data = lua_touserdata(ls_, nres_);

        if (NULL == arg_data)
        {
            char buff[512];
            snprintf(buff, sizeof(buff), "userdata<%s> param expected, but type<%s> provided",
                                         lua_type_info_t<T>::get_name(),
                                         lua_typename(ls_, lua_type(ls_, nres_)));
            printf("%s\n", buff);
            return -1;
        }

        if (0 == lua_getmetatable(ls_, nres_))
        {  //! does it have a metatable?
            return -1;
        }

        //! get correct metatable
        lua_getfield(ls_, LUA_REGISTRYINDEX, lua_type_info_t<T>::get_name());
        if (0 == lua_rawequal(ls_, -1, -2))
        {//! does it have the correct mt?
            char buff[512];
            snprintf(buff, sizeof(buff), "userdata<%s> param expected, but type<%s> provided",
                                         lua_type_info_t<T>::get_name(),
                                         lua_typename(ls_, lua_type(ls_, nres_)));
            printf("%s\n", buff);
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
        if (false == lua_type_info_t<T>::is_registed())
        {
            luaL_argerror(ls_, nres_, "arg can't convert to class*, because the class has not registed to Lua");
        }

        void* arg_data = luaL_checkudata(ls_, nres_, lua_type_info_t<T>::get_name());

        if (NULL == arg_data)
        {
            char buff[512];
            snprintf(buff, sizeof(buff), "`%s` expect arg 1, but arg == null",
                                         lua_type_info_t<T>::get_name());
            luaL_argerror(ls_, nres_, buff);
        }

        T* ret_ptr = ((class_userdata_t<T>*)arg_data)->obj_ptr;
        if (NULL == ret_ptr)
        {
            char buff[512];
            snprintf(buff, sizeof(buff), "`%s` object ptr can't be null",
                                         lua_type_info_t<T>::get_name());
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




#endif
