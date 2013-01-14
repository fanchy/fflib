#include <lua.hpp>
#include <string>
using namespace std;

#include <boost/function.hpp>

#include "lua_traits.h"

#define LUA_ARG_INDEX(x) ((x) + arg_index)

template <typename T, int arg_index = 0>
class function_dispather_traits_t;

template <int arg_index>
struct function_dispather_traits_t<void(*)(), arg_index>
{
    typedef void (*dest_func_t)();
    static  int closure_function(lua_State* ls_)
    {
        void* user_data = lua_touserdata (ls_, lua_upvalueindex(1));

        dest_func_t& registed_func = *((dest_func_t*)user_data);
        registed_func();
        return 0;
    }
};

template <int arg_index, typename ARG1>
struct function_dispather_traits_t<void(*)(ARG1), arg_index>
{
    typedef void (*dest_func_t)(ARG1);
    static  int closure_function(lua_State* ls_)
    {
        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::init_value();

        lua_traits_t<ARG1>::check_lua_param(ls_, LUA_ARG_INDEX(1), arg1);

        void* user_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_func_t& registed_func = *((dest_func_t*)user_data);

        registed_func(arg1);
        return 0;
    }
};

template <int arg_index, typename ARG1, typename ARG2>
struct function_dispather_traits_t<void(*)(ARG1, ARG2), arg_index>
{
    typedef void (*dest_func_t)(ARG1, ARG2);
    static  int closure_function(lua_State* ls_)
    {
        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::init_value();
        typename arg_traits_t<ARG2>::arg_type_t arg2 = init_value_traits_t<ARG2>::init_value();

        lua_traits_t<ARG1>::check_lua_param(ls_, LUA_ARG_INDEX(1), arg1);
        lua_traits_t<ARG2>::check_lua_param(ls_, LUA_ARG_INDEX(2), arg2);

        void* user_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_func_t& registed_func = *((dest_func_t*)user_data);

        registed_func(arg1, arg2);
        return 0;
    }
};

template <int arg_index, typename ARG1, typename ARG2, typename ARG3>
struct function_dispather_traits_t<void(*)(ARG1, ARG2, ARG3), arg_index>
{
    typedef void (*dest_func_t)(ARG1, ARG2, ARG3);
    static  int closure_function(lua_State* ls_)
    {
        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::init_value();
        typename arg_traits_t<ARG2>::arg_type_t arg2 = init_value_traits_t<ARG2>::init_value();
        typename arg_traits_t<ARG3>::arg_type_t arg3 = init_value_traits_t<ARG3>::init_value();

        lua_traits_t<ARG1>::check_lua_param(ls_, LUA_ARG_INDEX(1), arg1);
        lua_traits_t<ARG2>::check_lua_param(ls_, LUA_ARG_INDEX(2), arg2);
        lua_traits_t<ARG3>::check_lua_param(ls_, LUA_ARG_INDEX(3), arg3);

        void* user_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_func_t& registed_func = *((dest_func_t*)user_data);

        registed_func(arg1, arg2, arg3);
        return 0;
    }
};

template <int arg_index, typename ARG1, typename ARG2, typename ARG3, typename ARG4>
struct function_dispather_traits_t<void (*)(ARG1, ARG2, ARG3, ARG4), arg_index>
{
    typedef void (*dest_func_t)(ARG1, ARG2, ARG3, ARG4);
    static  int closure_function(lua_State* ls_)
    {
        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::init_value();
        typename arg_traits_t<ARG2>::arg_type_t arg2 = init_value_traits_t<ARG2>::init_value();
        typename arg_traits_t<ARG3>::arg_type_t arg3 = init_value_traits_t<ARG3>::init_value();
        typename arg_traits_t<ARG4>::arg_type_t arg4 = init_value_traits_t<ARG4>::init_value();

        lua_traits_t<ARG1>::check_lua_param(ls_, LUA_ARG_INDEX(1), arg1);
        lua_traits_t<ARG2>::check_lua_param(ls_, LUA_ARG_INDEX(2), arg2);
        lua_traits_t<ARG3>::check_lua_param(ls_, LUA_ARG_INDEX(3), arg3);
        lua_traits_t<ARG4>::check_lua_param(ls_, LUA_ARG_INDEX(4), arg4);

        void* user_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_func_t& registed_func = *((dest_func_t*)user_data);

        registed_func(arg1, arg2, arg3, arg4);
        return 0;
    }
};

template <int arg_index, typename ARG1, typename ARG2, typename ARG3, typename ARG4,
          typename ARG5>
struct function_dispather_traits_t<void (*)(ARG1, ARG2, ARG3, ARG4, ARG5), arg_index>
{
    typedef void (*dest_func_t)(ARG1, ARG2, ARG3, ARG4, ARG5);
    static  int closure_function(lua_State* ls_)
    {
        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::init_value();
        typename arg_traits_t<ARG2>::arg_type_t arg2 = init_value_traits_t<ARG2>::init_value();
        typename arg_traits_t<ARG3>::arg_type_t arg3 = init_value_traits_t<ARG3>::init_value();
        typename arg_traits_t<ARG4>::arg_type_t arg4 = init_value_traits_t<ARG4>::init_value();
        typename arg_traits_t<ARG5>::arg_type_t arg5 = init_value_traits_t<ARG5>::init_value();

        lua_traits_t<ARG1>::check_lua_param(ls_, LUA_ARG_INDEX(1), arg1);
        lua_traits_t<ARG2>::check_lua_param(ls_, LUA_ARG_INDEX(2), arg2);
        lua_traits_t<ARG3>::check_lua_param(ls_, LUA_ARG_INDEX(3), arg3);
        lua_traits_t<ARG4>::check_lua_param(ls_, LUA_ARG_INDEX(4), arg4);
        lua_traits_t<ARG5>::check_lua_param(ls_, LUA_ARG_INDEX(5), arg5);

        void* user_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_func_t& registed_func = *((dest_func_t*)user_data);

        registed_func(arg1, arg2, arg3, arg4, arg5);
        return 0;
    }
};
template <int arg_index, typename ARG1, typename ARG2, typename ARG3, typename ARG4,
          typename ARG5, typename ARG6>
struct function_dispather_traits_t<void (*)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6), arg_index>
{
    typedef void (*dest_func_t)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6);
    static  int closure_function(lua_State* ls_)
    {
        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::init_value();
        typename arg_traits_t<ARG2>::arg_type_t arg2 = init_value_traits_t<ARG2>::init_value();
        typename arg_traits_t<ARG3>::arg_type_t arg3 = init_value_traits_t<ARG3>::init_value();
        typename arg_traits_t<ARG4>::arg_type_t arg4 = init_value_traits_t<ARG4>::init_value();
        typename arg_traits_t<ARG5>::arg_type_t arg5 = init_value_traits_t<ARG5>::init_value();
        typename arg_traits_t<ARG6>::arg_type_t arg6 = init_value_traits_t<ARG6>::init_value();

        lua_traits_t<ARG1>::check_lua_param(ls_, LUA_ARG_INDEX(1), arg1);
        lua_traits_t<ARG2>::check_lua_param(ls_, LUA_ARG_INDEX(2), arg2);
        lua_traits_t<ARG3>::check_lua_param(ls_, LUA_ARG_INDEX(3), arg3);
        lua_traits_t<ARG4>::check_lua_param(ls_, LUA_ARG_INDEX(4), arg4);
        lua_traits_t<ARG5>::check_lua_param(ls_, LUA_ARG_INDEX(5), arg5);
        lua_traits_t<ARG6>::check_lua_param(ls_, LUA_ARG_INDEX(6), arg6);

        void* user_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_func_t& registed_func = *((dest_func_t*)user_data);

        registed_func(arg1, arg2, arg3, arg4, arg5, arg6);
        return 0;
    }
};

template <int arg_index, typename ARG1, typename ARG2, typename ARG3, typename ARG4,
          typename ARG5, typename ARG6, typename ARG7>
struct function_dispather_traits_t<void (*)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7), arg_index>
{
    typedef void (*dest_func_t)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7);
    static  int closure_function(lua_State* ls_)
    {
        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::init_value();
        typename arg_traits_t<ARG2>::arg_type_t arg2 = init_value_traits_t<ARG2>::init_value();
        typename arg_traits_t<ARG3>::arg_type_t arg3 = init_value_traits_t<ARG3>::init_value();
        typename arg_traits_t<ARG4>::arg_type_t arg4 = init_value_traits_t<ARG4>::init_value();
        typename arg_traits_t<ARG5>::arg_type_t arg5 = init_value_traits_t<ARG5>::init_value();
        typename arg_traits_t<ARG6>::arg_type_t arg6 = init_value_traits_t<ARG6>::init_value();
        typename arg_traits_t<ARG7>::arg_type_t arg7 = init_value_traits_t<ARG7>::init_value();

        lua_traits_t<ARG1>::check_lua_param(ls_, LUA_ARG_INDEX(1), arg1);
        lua_traits_t<ARG2>::check_lua_param(ls_, LUA_ARG_INDEX(2), arg2);
        lua_traits_t<ARG3>::check_lua_param(ls_, LUA_ARG_INDEX(3), arg3);
        lua_traits_t<ARG4>::check_lua_param(ls_, LUA_ARG_INDEX(4), arg4);
        lua_traits_t<ARG5>::check_lua_param(ls_, LUA_ARG_INDEX(5), arg5);
        lua_traits_t<ARG6>::check_lua_param(ls_, LUA_ARG_INDEX(6), arg6);
        lua_traits_t<ARG7>::check_lua_param(ls_, LUA_ARG_INDEX(7), arg7);

        void* user_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_func_t& registed_func = *((dest_func_t*)user_data);

        registed_func(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
        return 0;
    }
};

template <int arg_index, typename ARG1, typename ARG2, typename ARG3, typename ARG4,
          typename ARG5, typename ARG6, typename ARG7, typename ARG8>
struct function_dispather_traits_t<void (*)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8), arg_index>
{
    typedef void (*dest_func_t)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8);
    static  int closure_function(lua_State* ls_)
    {
        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::init_value();
        typename arg_traits_t<ARG2>::arg_type_t arg2 = init_value_traits_t<ARG2>::init_value();
        typename arg_traits_t<ARG3>::arg_type_t arg3 = init_value_traits_t<ARG3>::init_value();
        typename arg_traits_t<ARG4>::arg_type_t arg4 = init_value_traits_t<ARG4>::init_value();
        typename arg_traits_t<ARG5>::arg_type_t arg5 = init_value_traits_t<ARG5>::init_value();
        typename arg_traits_t<ARG6>::arg_type_t arg6 = init_value_traits_t<ARG6>::init_value();
        typename arg_traits_t<ARG7>::arg_type_t arg7 = init_value_traits_t<ARG7>::init_value();
        typename arg_traits_t<ARG8>::arg_type_t arg8 = init_value_traits_t<ARG8>::init_value();

        lua_traits_t<ARG1>::check_lua_param(ls_, LUA_ARG_INDEX(1), arg1);
        lua_traits_t<ARG2>::check_lua_param(ls_, LUA_ARG_INDEX(2), arg2);
        lua_traits_t<ARG3>::check_lua_param(ls_, LUA_ARG_INDEX(3), arg3);
        lua_traits_t<ARG4>::check_lua_param(ls_, LUA_ARG_INDEX(4), arg4);
        lua_traits_t<ARG5>::check_lua_param(ls_, LUA_ARG_INDEX(5), arg5);
        lua_traits_t<ARG6>::check_lua_param(ls_, LUA_ARG_INDEX(6), arg6);
        lua_traits_t<ARG7>::check_lua_param(ls_, LUA_ARG_INDEX(7), arg7);
        lua_traits_t<ARG8>::check_lua_param(ls_, LUA_ARG_INDEX(8), arg8);

        void* user_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_func_t& registed_func = *((dest_func_t*)user_data);

        registed_func(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
        return 0;
    }
};


template <int arg_index, typename ARG1, typename ARG2, typename ARG3, typename ARG4,
          typename ARG5, typename ARG6, typename ARG7, typename ARG8, typename ARG9>
struct function_dispather_traits_t<void (*)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8, ARG9), arg_index>
{
    typedef void (*dest_func_t)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8, ARG9);
    static  int closure_function(lua_State* ls_)
    {
        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::init_value();
        typename arg_traits_t<ARG2>::arg_type_t arg2 = init_value_traits_t<ARG2>::init_value();
        typename arg_traits_t<ARG3>::arg_type_t arg3 = init_value_traits_t<ARG3>::init_value();
        typename arg_traits_t<ARG4>::arg_type_t arg4 = init_value_traits_t<ARG4>::init_value();
        typename arg_traits_t<ARG5>::arg_type_t arg5 = init_value_traits_t<ARG5>::init_value();
        typename arg_traits_t<ARG6>::arg_type_t arg6 = init_value_traits_t<ARG6>::init_value();
        typename arg_traits_t<ARG7>::arg_type_t arg7 = init_value_traits_t<ARG7>::init_value();
        typename arg_traits_t<ARG8>::arg_type_t arg8 = init_value_traits_t<ARG8>::init_value();
        typename arg_traits_t<ARG9>::arg_type_t arg9 = init_value_traits_t<ARG9>::init_value();

        lua_traits_t<ARG1>::check_lua_param(ls_, LUA_ARG_INDEX(1), arg1);
        lua_traits_t<ARG2>::check_lua_param(ls_, LUA_ARG_INDEX(2), arg2);
        lua_traits_t<ARG3>::check_lua_param(ls_, LUA_ARG_INDEX(3), arg3);
        lua_traits_t<ARG4>::check_lua_param(ls_, LUA_ARG_INDEX(4), arg4);
        lua_traits_t<ARG5>::check_lua_param(ls_, LUA_ARG_INDEX(5), arg5);
        lua_traits_t<ARG6>::check_lua_param(ls_, LUA_ARG_INDEX(6), arg6);
        lua_traits_t<ARG7>::check_lua_param(ls_, LUA_ARG_INDEX(7), arg7);
        lua_traits_t<ARG8>::check_lua_param(ls_, LUA_ARG_INDEX(8), arg8);
        lua_traits_t<ARG9>::check_lua_param(ls_, LUA_ARG_INDEX(9), arg9);

        void* user_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_func_t& registed_func = *((dest_func_t*)user_data);

        registed_func(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
        return 0;
    }
};

template <typename RET, int arg_index>
struct function_dispather_traits_t<RET (*)(), arg_index>
{
    typedef RET (*dest_func_t)();
    static  int closure_function(lua_State* ls_)
    {
        void* user_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_func_t& registed_func = *((dest_func_t*)user_data);

        RET ret = registed_func();
        lua_traits_t<RET>::push_stack(ls_, ret);

        return 1;
    }
};

template <int arg_index, typename RET, typename ARG1>
struct function_dispather_traits_t<RET (*)(ARG1), arg_index>
{
    typedef RET (*dest_func_t)(ARG1);
    static  int closure_function(lua_State* ls_)
    {
        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::init_value();
        lua_traits_t<ARG1>::check_lua_param(ls_, LUA_ARG_INDEX(1), arg1);

        void* user_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_func_t& registed_func = *((dest_func_t*)user_data);

        RET ret = registed_func(arg1);
        lua_traits_t<RET>::push_stack(ls_, ret);
        return 1;
    }
};

template <int arg_index, typename RET, typename ARG1, typename ARG2>
struct function_dispather_traits_t<RET (*)(ARG1, ARG2), arg_index>
{
    typedef RET (*dest_func_t)(ARG1, ARG2);
    static  int closure_function(lua_State* ls_)
    {
        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::init_value();
        typename arg_traits_t<ARG2>::arg_type_t arg2 = init_value_traits_t<ARG2>::init_value();
        lua_traits_t<ARG1>::check_lua_param(ls_, LUA_ARG_INDEX(1), arg1);
        lua_traits_t<ARG2>::check_lua_param(ls_, LUA_ARG_INDEX(2), arg2);

        void* user_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_func_t& registed_func = *((dest_func_t*)user_data);

        RET ret = registed_func(arg1, arg2);
        lua_traits_t<RET>::push_stack(ls_, ret);
        return 1;
    }
};

template <int arg_index, typename RET, typename ARG1, typename ARG2, typename ARG3>
struct function_dispather_traits_t<RET (*)(ARG1, ARG2, ARG3), arg_index>
{
    typedef RET (*dest_func_t)(ARG1, ARG2, ARG3);
    static  int closure_function(lua_State* ls_)
    {
        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::init_value();
        typename arg_traits_t<ARG2>::arg_type_t arg2 = init_value_traits_t<ARG2>::init_value();
        typename arg_traits_t<ARG3>::arg_type_t arg3 = init_value_traits_t<ARG3>::init_value();

        lua_traits_t<ARG1>::check_lua_param(ls_, LUA_ARG_INDEX(1), arg1);
        lua_traits_t<ARG2>::check_lua_param(ls_, LUA_ARG_INDEX(2), arg2);
        lua_traits_t<ARG3>::check_lua_param(ls_, LUA_ARG_INDEX(3), arg3);

        void* user_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_func_t& registed_func = *((dest_func_t*)user_data);

        RET ret = registed_func(arg1, arg2, arg3);
        lua_traits_t<RET>::push_stack(ls_, ret);
        return 1;
    }
};

template <int arg_index, typename RET, typename ARG1, typename ARG2, typename ARG3, typename ARG4>
struct function_dispather_traits_t<RET (*)(ARG1, ARG2, ARG3, ARG4), arg_index>
{
    typedef RET (*dest_func_t)(ARG1, ARG2, ARG3, ARG4);
    static  int closure_function(lua_State* ls_)
    {
        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::init_value();
        typename arg_traits_t<ARG2>::arg_type_t arg2 = init_value_traits_t<ARG2>::init_value();
        typename arg_traits_t<ARG3>::arg_type_t arg3 = init_value_traits_t<ARG3>::init_value();
        typename arg_traits_t<ARG4>::arg_type_t arg4 = init_value_traits_t<ARG4>::init_value();

        lua_traits_t<ARG1>::check_lua_param(ls_, LUA_ARG_INDEX(1), arg1);
        lua_traits_t<ARG2>::check_lua_param(ls_, LUA_ARG_INDEX(2), arg2);
        lua_traits_t<ARG3>::check_lua_param(ls_, LUA_ARG_INDEX(3), arg3);
        lua_traits_t<ARG4>::check_lua_param(ls_, LUA_ARG_INDEX(4), arg4);

        void* user_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_func_t& registed_func = *((dest_func_t*)user_data);

        RET ret = registed_func(arg1, arg2, arg3, arg4);
        lua_traits_t<RET>::push_stack(ls_, ret);
        return 1;
    }
};

template <int arg_index, typename RET, typename ARG1, typename ARG2, typename ARG3, typename ARG4,
          typename ARG5>
struct function_dispather_traits_t<RET (*)(ARG1, ARG2, ARG3, ARG4, ARG5), arg_index>
{
    typedef RET (*dest_func_t)(ARG1, ARG2, ARG3, ARG4, ARG5);
    static  int closure_function(lua_State* ls_)
    {
        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::init_value();
        typename arg_traits_t<ARG2>::arg_type_t arg2 = init_value_traits_t<ARG2>::init_value();
        typename arg_traits_t<ARG3>::arg_type_t arg3 = init_value_traits_t<ARG3>::init_value();
        typename arg_traits_t<ARG4>::arg_type_t arg4 = init_value_traits_t<ARG4>::init_value();
        typename arg_traits_t<ARG5>::arg_type_t arg5 = init_value_traits_t<ARG5>::init_value();

        lua_traits_t<ARG1>::check_lua_param(ls_, LUA_ARG_INDEX(1), arg1);
        lua_traits_t<ARG2>::check_lua_param(ls_, LUA_ARG_INDEX(2), arg2);
        lua_traits_t<ARG3>::check_lua_param(ls_, LUA_ARG_INDEX(3), arg3);
        lua_traits_t<ARG4>::check_lua_param(ls_, LUA_ARG_INDEX(4), arg4);
        lua_traits_t<ARG5>::check_lua_param(ls_, LUA_ARG_INDEX(5), arg5);

        void* user_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_func_t& registed_func = *((dest_func_t*)user_data);

        RET ret = registed_func(arg1, arg2, arg3, arg4, arg5);
        lua_traits_t<RET>::push_stack(ls_, ret);
        return 1;
    }
};


template <int arg_index, typename RET, typename ARG1, typename ARG2, typename ARG3, typename ARG4,
          typename ARG5, typename ARG6>
struct function_dispather_traits_t<RET (*)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6), arg_index>
{
    typedef RET (*dest_func_t)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6);
    static  int closure_function(lua_State* ls_)
    {
        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::init_value();
        typename arg_traits_t<ARG2>::arg_type_t arg2 = init_value_traits_t<ARG2>::init_value();
        typename arg_traits_t<ARG3>::arg_type_t arg3 = init_value_traits_t<ARG3>::init_value();
        typename arg_traits_t<ARG4>::arg_type_t arg4 = init_value_traits_t<ARG4>::init_value();
        typename arg_traits_t<ARG5>::arg_type_t arg5 = init_value_traits_t<ARG5>::init_value();
        typename arg_traits_t<ARG6>::arg_type_t arg6 = init_value_traits_t<ARG6>::init_value();

        lua_traits_t<ARG1>::check_lua_param(ls_, LUA_ARG_INDEX(1), arg1);
        lua_traits_t<ARG2>::check_lua_param(ls_, LUA_ARG_INDEX(2), arg2);
        lua_traits_t<ARG3>::check_lua_param(ls_, LUA_ARG_INDEX(3), arg3);
        lua_traits_t<ARG4>::check_lua_param(ls_, LUA_ARG_INDEX(4), arg4);
        lua_traits_t<ARG5>::check_lua_param(ls_, LUA_ARG_INDEX(5), arg5);
        lua_traits_t<ARG6>::check_lua_param(ls_, LUA_ARG_INDEX(6), arg6);

        void* user_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_func_t& registed_func = *((dest_func_t*)user_data);

        RET ret = registed_func(arg1, arg2, arg3, arg4, arg5, arg6);
        lua_traits_t<RET>::push_stack(ls_, ret);
        return 1;
    }
};

template <int arg_index, typename RET, typename ARG1, typename ARG2, typename ARG3, typename ARG4,
          typename ARG5, typename ARG6, typename ARG7>
struct function_dispather_traits_t<RET (*)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7), arg_index>
{
    typedef RET (*dest_func_t)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7);
    static  int closure_function(lua_State* ls_)
    {
        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::init_value();
        typename arg_traits_t<ARG2>::arg_type_t arg2 = init_value_traits_t<ARG2>::init_value();
        typename arg_traits_t<ARG3>::arg_type_t arg3 = init_value_traits_t<ARG3>::init_value();
        typename arg_traits_t<ARG4>::arg_type_t arg4 = init_value_traits_t<ARG4>::init_value();
        typename arg_traits_t<ARG5>::arg_type_t arg5 = init_value_traits_t<ARG5>::init_value();
        typename arg_traits_t<ARG6>::arg_type_t arg6 = init_value_traits_t<ARG6>::init_value();
        typename arg_traits_t<ARG7>::arg_type_t arg7 = init_value_traits_t<ARG7>::init_value();

        lua_traits_t<ARG1>::check_lua_param(ls_, LUA_ARG_INDEX(1), arg1);
        lua_traits_t<ARG2>::check_lua_param(ls_, LUA_ARG_INDEX(2), arg2);
        lua_traits_t<ARG3>::check_lua_param(ls_, LUA_ARG_INDEX(3), arg3);
        lua_traits_t<ARG4>::check_lua_param(ls_, LUA_ARG_INDEX(4), arg4);
        lua_traits_t<ARG5>::check_lua_param(ls_, LUA_ARG_INDEX(5), arg5);
        lua_traits_t<ARG6>::check_lua_param(ls_, LUA_ARG_INDEX(6), arg6);
        lua_traits_t<ARG7>::check_lua_param(ls_, LUA_ARG_INDEX(7), arg7);

        void* user_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_func_t& registed_func = *((dest_func_t*)user_data);

        RET ret = registed_func(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
        lua_traits_t<RET>::push_stack(ls_, ret);
        return 1;
    }
};

template <int arg_index, typename RET, typename ARG1, typename ARG2, typename ARG3, typename ARG4,
          typename ARG5, typename ARG6, typename ARG7, typename ARG8>
struct function_dispather_traits_t<RET (*)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8), arg_index>
{
    typedef RET (*dest_func_t)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8);
    static  int closure_function(lua_State* ls_)
    {
        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::init_value();
        typename arg_traits_t<ARG2>::arg_type_t arg2 = init_value_traits_t<ARG2>::init_value();
        typename arg_traits_t<ARG3>::arg_type_t arg3 = init_value_traits_t<ARG3>::init_value();
        typename arg_traits_t<ARG4>::arg_type_t arg4 = init_value_traits_t<ARG4>::init_value();
        typename arg_traits_t<ARG5>::arg_type_t arg5 = init_value_traits_t<ARG5>::init_value();
        typename arg_traits_t<ARG6>::arg_type_t arg6 = init_value_traits_t<ARG6>::init_value();
        typename arg_traits_t<ARG7>::arg_type_t arg7 = init_value_traits_t<ARG7>::init_value();
        typename arg_traits_t<ARG8>::arg_type_t arg8 = init_value_traits_t<ARG8>::init_value();

        lua_traits_t<ARG1>::check_lua_param(ls_, LUA_ARG_INDEX(1), arg1);
        lua_traits_t<ARG2>::check_lua_param(ls_, LUA_ARG_INDEX(2), arg2);
        lua_traits_t<ARG3>::check_lua_param(ls_, LUA_ARG_INDEX(3), arg3);
        lua_traits_t<ARG4>::check_lua_param(ls_, LUA_ARG_INDEX(4), arg4);
        lua_traits_t<ARG5>::check_lua_param(ls_, LUA_ARG_INDEX(5), arg5);
        lua_traits_t<ARG6>::check_lua_param(ls_, LUA_ARG_INDEX(6), arg6);
        lua_traits_t<ARG7>::check_lua_param(ls_, LUA_ARG_INDEX(7), arg7);
        lua_traits_t<ARG8>::check_lua_param(ls_, LUA_ARG_INDEX(8), arg8);

        void* user_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_func_t& registed_func = *((dest_func_t*)user_data);

        RET ret = registed_func(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
        lua_traits_t<RET>::push_stack(ls_, ret);
        return 1;
    }
};

template <int arg_index, typename RET, typename ARG1, typename ARG2, typename ARG3, typename ARG4,
          typename ARG5, typename ARG6, typename ARG7, typename ARG8, typename ARG9>
struct function_dispather_traits_t<RET (*)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8, ARG9), arg_index>
{
    typedef RET (*dest_func_t)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8, ARG9);
    static  int closure_function(lua_State* ls_)
    {
        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::init_value();
        typename arg_traits_t<ARG2>::arg_type_t arg2 = init_value_traits_t<ARG2>::init_value();
        typename arg_traits_t<ARG3>::arg_type_t arg3 = init_value_traits_t<ARG3>::init_value();
        typename arg_traits_t<ARG4>::arg_type_t arg4 = init_value_traits_t<ARG4>::init_value();
        typename arg_traits_t<ARG5>::arg_type_t arg5 = init_value_traits_t<ARG5>::init_value();
        typename arg_traits_t<ARG6>::arg_type_t arg6 = init_value_traits_t<ARG6>::init_value();
        typename arg_traits_t<ARG7>::arg_type_t arg7 = init_value_traits_t<ARG7>::init_value();
        typename arg_traits_t<ARG8>::arg_type_t arg8 = init_value_traits_t<ARG8>::init_value();
        typename arg_traits_t<ARG9>::arg_type_t arg9 = init_value_traits_t<ARG9>::init_value();

        lua_traits_t<ARG1>::check_lua_param(ls_, LUA_ARG_INDEX(1), arg1);
        lua_traits_t<ARG2>::check_lua_param(ls_, LUA_ARG_INDEX(2), arg2);
        lua_traits_t<ARG3>::check_lua_param(ls_, LUA_ARG_INDEX(3), arg3);
        lua_traits_t<ARG4>::check_lua_param(ls_, LUA_ARG_INDEX(4), arg4);
        lua_traits_t<ARG5>::check_lua_param(ls_, LUA_ARG_INDEX(5), arg5);
        lua_traits_t<ARG6>::check_lua_param(ls_, LUA_ARG_INDEX(6), arg6);
        lua_traits_t<ARG7>::check_lua_param(ls_, LUA_ARG_INDEX(7), arg7);
        lua_traits_t<ARG8>::check_lua_param(ls_, LUA_ARG_INDEX(8), arg8);
        lua_traits_t<ARG9>::check_lua_param(ls_, LUA_ARG_INDEX(9), arg9);

        void* user_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_func_t& registed_func = *((dest_func_t*)user_data);

        RET ret = registed_func(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
        lua_traits_t<RET>::push_stack(ls_, ret);
        return 1;
    }
};


template <>
struct function_dispather_traits_t<boost::function<void ()> >
{
    typedef boost::function<void ()> dest_func_t;
    static  int closure_function(lua_State* ls_)
    {
        void* user_data = lua_touserdata (ls_, lua_upvalueindex(1));

        dest_func_t& registed_func = *((dest_func_t*)user_data);
        registed_func();
        return 0;
    }
};

template <int arg_index, typename ARG1>
struct function_dispather_traits_t<boost::function<void (ARG1)>, arg_index>
{
    typedef boost::function<void (ARG1)> dest_func_t;
    static  int closure_function(lua_State* ls_)
    {
        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::init_value();

        lua_traits_t<ARG1>::check_lua_param(ls_, LUA_ARG_INDEX(1), arg1);

        void* user_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_func_t& registed_func = *((dest_func_t*)user_data);

        registed_func(arg1);
        return 0;
    }
};

template <int arg_index, typename ARG1, typename ARG2>
struct function_dispather_traits_t<boost::function<void (ARG1, ARG2)>, arg_index>
{
    typedef boost::function<void (ARG1, ARG2)> dest_func_t;
    static  int closure_function(lua_State* ls_)
    {
        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::init_value();
        typename arg_traits_t<ARG2>::arg_type_t arg2 = init_value_traits_t<ARG2>::init_value();

        lua_traits_t<ARG1>::check_lua_param(ls_, LUA_ARG_INDEX(1), arg1);
        lua_traits_t<ARG2>::check_lua_param(ls_, LUA_ARG_INDEX(2), arg2);

        void* user_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_func_t& registed_func = *((dest_func_t*)user_data);

        registed_func(arg1, arg2);
        return 0;
    }
};

template <int arg_index, typename ARG1, typename ARG2, typename ARG3>
struct function_dispather_traits_t<boost::function<void (ARG1, ARG2, ARG3)>, arg_index>
{
    typedef boost::function<void (ARG1, ARG2, ARG3)> dest_func_t;
    static  int closure_function(lua_State* ls_)
    {
        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::init_value();
        typename arg_traits_t<ARG2>::arg_type_t arg2 = init_value_traits_t<ARG2>::init_value();
        typename arg_traits_t<ARG3>::arg_type_t arg3 = init_value_traits_t<ARG3>::init_value();

        lua_traits_t<ARG1>::check_lua_param(ls_, LUA_ARG_INDEX(1), arg1);
        lua_traits_t<ARG2>::check_lua_param(ls_, LUA_ARG_INDEX(2), arg2);
        lua_traits_t<ARG3>::check_lua_param(ls_, LUA_ARG_INDEX(3), arg3);

        void* user_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_func_t& registed_func = *((dest_func_t*)user_data);

        registed_func(arg1, arg2, arg3);
        return 0;
    }
};

template <int arg_index, typename ARG1, typename ARG2, typename ARG3, typename ARG4>
struct function_dispather_traits_t<boost::function<void (ARG1, ARG2, ARG3, ARG4)>, arg_index>
{
    typedef boost::function<void (ARG1, ARG2, ARG3, ARG4)> dest_func_t;
    static  int closure_function(lua_State* ls_)
    {
        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::init_value();
        typename arg_traits_t<ARG2>::arg_type_t arg2 = init_value_traits_t<ARG2>::init_value();
        typename arg_traits_t<ARG3>::arg_type_t arg3 = init_value_traits_t<ARG3>::init_value();
        typename arg_traits_t<ARG4>::arg_type_t arg4 = init_value_traits_t<ARG4>::init_value();

        lua_traits_t<ARG1>::check_lua_param(ls_, LUA_ARG_INDEX(1), arg1);
        lua_traits_t<ARG2>::check_lua_param(ls_, LUA_ARG_INDEX(2), arg2);
        lua_traits_t<ARG3>::check_lua_param(ls_, LUA_ARG_INDEX(3), arg3);
        lua_traits_t<ARG4>::check_lua_param(ls_, LUA_ARG_INDEX(4), arg4);

        void* user_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_func_t& registed_func = *((dest_func_t*)user_data);

        registed_func(arg1, arg2, arg3, arg4);
        return 0;
    }
};

template <int arg_index, typename ARG1, typename ARG2, typename ARG3, typename ARG4,
          typename ARG5>
struct function_dispather_traits_t<boost::function<void (ARG1, ARG2, ARG3, ARG4, ARG5)>, arg_index>
{
    typedef boost::function<void (ARG1, ARG2, ARG3, ARG4, ARG5)> dest_func_t;
    static  int closure_function(lua_State* ls_)
    {
        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::init_value();
        typename arg_traits_t<ARG2>::arg_type_t arg2 = init_value_traits_t<ARG2>::init_value();
        typename arg_traits_t<ARG3>::arg_type_t arg3 = init_value_traits_t<ARG3>::init_value();
        typename arg_traits_t<ARG4>::arg_type_t arg4 = init_value_traits_t<ARG4>::init_value();
        typename arg_traits_t<ARG5>::arg_type_t arg5 = init_value_traits_t<ARG5>::init_value();

        lua_traits_t<ARG1>::check_lua_param(ls_, LUA_ARG_INDEX(1), arg1);
        lua_traits_t<ARG2>::check_lua_param(ls_, LUA_ARG_INDEX(2), arg2);
        lua_traits_t<ARG3>::check_lua_param(ls_, LUA_ARG_INDEX(3), arg3);
        lua_traits_t<ARG4>::check_lua_param(ls_, LUA_ARG_INDEX(4), arg4);
        lua_traits_t<ARG5>::check_lua_param(ls_, LUA_ARG_INDEX(5), arg5);

        void* user_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_func_t& registed_func = *((dest_func_t*)user_data);

        registed_func(arg1, arg2, arg3, arg4, arg5);
        return 0;
    }
};
template <int arg_index, typename ARG1, typename ARG2, typename ARG3, typename ARG4,
          typename ARG5, typename ARG6>
struct function_dispather_traits_t<boost::function<void (ARG1, ARG2, ARG3, ARG4, ARG5, ARG6)>, arg_index>
{
    typedef boost::function<void (ARG1, ARG2, ARG3, ARG4, ARG5, ARG6)> dest_func_t;
    static  int closure_function(lua_State* ls_)
    {
        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::init_value();
        typename arg_traits_t<ARG2>::arg_type_t arg2 = init_value_traits_t<ARG2>::init_value();
        typename arg_traits_t<ARG3>::arg_type_t arg3 = init_value_traits_t<ARG3>::init_value();
        typename arg_traits_t<ARG4>::arg_type_t arg4 = init_value_traits_t<ARG4>::init_value();
        typename arg_traits_t<ARG5>::arg_type_t arg5 = init_value_traits_t<ARG5>::init_value();
        typename arg_traits_t<ARG6>::arg_type_t arg6 = init_value_traits_t<ARG6>::init_value();

        lua_traits_t<ARG1>::check_lua_param(ls_, LUA_ARG_INDEX(1), arg1);
        lua_traits_t<ARG2>::check_lua_param(ls_, LUA_ARG_INDEX(2), arg2);
        lua_traits_t<ARG3>::check_lua_param(ls_, LUA_ARG_INDEX(3), arg3);
        lua_traits_t<ARG4>::check_lua_param(ls_, LUA_ARG_INDEX(4), arg4);
        lua_traits_t<ARG5>::check_lua_param(ls_, LUA_ARG_INDEX(5), arg5);
        lua_traits_t<ARG6>::check_lua_param(ls_, LUA_ARG_INDEX(6), arg6);

        void* user_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_func_t& registed_func = *((dest_func_t*)user_data);

        registed_func(arg1, arg2, arg3, arg4, arg5, arg6);
        return 0;
    }
};


template <int arg_index, typename ARG1, typename ARG2, typename ARG3, typename ARG4,
          typename ARG5, typename ARG6, typename ARG7>
struct function_dispather_traits_t<boost::function<void (ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7)>, arg_index>
{
    typedef boost::function<void (ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7)> dest_func_t;
    static  int closure_function(lua_State* ls_)
    {
        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::init_value();
        typename arg_traits_t<ARG2>::arg_type_t arg2 = init_value_traits_t<ARG2>::init_value();
        typename arg_traits_t<ARG3>::arg_type_t arg3 = init_value_traits_t<ARG3>::init_value();
        typename arg_traits_t<ARG4>::arg_type_t arg4 = init_value_traits_t<ARG4>::init_value();
        typename arg_traits_t<ARG5>::arg_type_t arg5 = init_value_traits_t<ARG5>::init_value();
        typename arg_traits_t<ARG6>::arg_type_t arg6 = init_value_traits_t<ARG6>::init_value();
        typename arg_traits_t<ARG7>::arg_type_t arg7 = init_value_traits_t<ARG7>::init_value();

        lua_traits_t<ARG1>::check_lua_param(ls_, LUA_ARG_INDEX(1), arg1);
        lua_traits_t<ARG2>::check_lua_param(ls_, LUA_ARG_INDEX(2), arg2);
        lua_traits_t<ARG3>::check_lua_param(ls_, LUA_ARG_INDEX(3), arg3);
        lua_traits_t<ARG4>::check_lua_param(ls_, LUA_ARG_INDEX(4), arg4);
        lua_traits_t<ARG5>::check_lua_param(ls_, LUA_ARG_INDEX(5), arg5);
        lua_traits_t<ARG6>::check_lua_param(ls_, LUA_ARG_INDEX(6), arg6);
        lua_traits_t<ARG7>::check_lua_param(ls_, LUA_ARG_INDEX(7), arg7);

        void* user_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_func_t& registed_func = *((dest_func_t*)user_data);

        registed_func(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
        return 0;
    }
};

template <int arg_index, typename ARG1, typename ARG2, typename ARG3, typename ARG4,
          typename ARG5, typename ARG6, typename ARG7, typename ARG8>
struct function_dispather_traits_t<boost::function<void (ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8)>, arg_index>
{
    typedef boost::function<void (ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8)> dest_func_t;
    static  int closure_function(lua_State* ls_)
    {
        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::init_value();
        typename arg_traits_t<ARG2>::arg_type_t arg2 = init_value_traits_t<ARG2>::init_value();
        typename arg_traits_t<ARG3>::arg_type_t arg3 = init_value_traits_t<ARG3>::init_value();
        typename arg_traits_t<ARG4>::arg_type_t arg4 = init_value_traits_t<ARG4>::init_value();
        typename arg_traits_t<ARG5>::arg_type_t arg5 = init_value_traits_t<ARG5>::init_value();
        typename arg_traits_t<ARG6>::arg_type_t arg6 = init_value_traits_t<ARG6>::init_value();
        typename arg_traits_t<ARG7>::arg_type_t arg7 = init_value_traits_t<ARG7>::init_value();
        typename arg_traits_t<ARG8>::arg_type_t arg8 = init_value_traits_t<ARG8>::init_value();

        lua_traits_t<ARG1>::check_lua_param(ls_, LUA_ARG_INDEX(1), arg1);
        lua_traits_t<ARG2>::check_lua_param(ls_, LUA_ARG_INDEX(2), arg2);
        lua_traits_t<ARG3>::check_lua_param(ls_, LUA_ARG_INDEX(3), arg3);
        lua_traits_t<ARG4>::check_lua_param(ls_, LUA_ARG_INDEX(4), arg4);
        lua_traits_t<ARG5>::check_lua_param(ls_, LUA_ARG_INDEX(5), arg5);
        lua_traits_t<ARG6>::check_lua_param(ls_, LUA_ARG_INDEX(6), arg6);
        lua_traits_t<ARG7>::check_lua_param(ls_, LUA_ARG_INDEX(7), arg7);
        lua_traits_t<ARG8>::check_lua_param(ls_, LUA_ARG_INDEX(8), arg8);

        void* user_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_func_t& registed_func = *((dest_func_t*)user_data);

        registed_func(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
        return 0;
    }
};


template <int arg_index, typename ARG1, typename ARG2, typename ARG3, typename ARG4,
          typename ARG5, typename ARG6, typename ARG7, typename ARG8, typename ARG9>
struct function_dispather_traits_t<boost::function<void (ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8, ARG9)>, arg_index>
{
    typedef boost::function<void (ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8, ARG9)> dest_func_t;
    static  int closure_function(lua_State* ls_)
    {
        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::init_value();
        typename arg_traits_t<ARG2>::arg_type_t arg2 = init_value_traits_t<ARG2>::init_value();
        typename arg_traits_t<ARG3>::arg_type_t arg3 = init_value_traits_t<ARG3>::init_value();
        typename arg_traits_t<ARG4>::arg_type_t arg4 = init_value_traits_t<ARG4>::init_value();
        typename arg_traits_t<ARG5>::arg_type_t arg5 = init_value_traits_t<ARG5>::init_value();
        typename arg_traits_t<ARG6>::arg_type_t arg6 = init_value_traits_t<ARG6>::init_value();
        typename arg_traits_t<ARG7>::arg_type_t arg7 = init_value_traits_t<ARG7>::init_value();
        typename arg_traits_t<ARG8>::arg_type_t arg8 = init_value_traits_t<ARG8>::init_value();
        typename arg_traits_t<ARG9>::arg_type_t arg9 = init_value_traits_t<ARG9>::init_value();

        lua_traits_t<ARG1>::check_lua_param(ls_, LUA_ARG_INDEX(1), arg1);
        lua_traits_t<ARG2>::check_lua_param(ls_, LUA_ARG_INDEX(2), arg2);
        lua_traits_t<ARG3>::check_lua_param(ls_, LUA_ARG_INDEX(3), arg3);
        lua_traits_t<ARG4>::check_lua_param(ls_, LUA_ARG_INDEX(4), arg4);
        lua_traits_t<ARG5>::check_lua_param(ls_, LUA_ARG_INDEX(5), arg5);
        lua_traits_t<ARG6>::check_lua_param(ls_, LUA_ARG_INDEX(6), arg6);
        lua_traits_t<ARG7>::check_lua_param(ls_, LUA_ARG_INDEX(7), arg7);
        lua_traits_t<ARG8>::check_lua_param(ls_, LUA_ARG_INDEX(8), arg8);
        lua_traits_t<ARG9>::check_lua_param(ls_, LUA_ARG_INDEX(9), arg9);

        void* user_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_func_t& registed_func = *((dest_func_t*)user_data);

        registed_func(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
        return 0;
    }
};

template <typename RET>
struct function_dispather_traits_t<boost::function<RET ()> >
{
    typedef boost::function<RET ()> dest_func_t;
    static  int closure_function(lua_State* ls_)
    {
        void* user_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_func_t& registed_func = *((dest_func_t*)user_data);

        RET ret = registed_func();
        lua_traits_t<RET>::push_stack(ls_, ret);

        return 1;
    }
};

template <int arg_index, typename RET, typename ARG1>
struct function_dispather_traits_t<boost::function<RET (ARG1)>, arg_index>
{
    typedef boost::function<RET (ARG1)> dest_func_t;
    static  int closure_function(lua_State* ls_)
    {
        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::init_value();
        lua_traits_t<ARG1>::check_lua_param(ls_, LUA_ARG_INDEX(1), arg1);

        void* user_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_func_t& registed_func = *((dest_func_t*)user_data);

        RET ret = registed_func(arg1);
        lua_traits_t<RET>::push_stack(ls_, ret);
        return 1;
    }
};

template <int arg_index, typename RET, typename ARG1, typename ARG2>
struct function_dispather_traits_t<boost::function<RET (ARG1, ARG2)>, arg_index>
{
    typedef boost::function<RET (ARG1, ARG2)> dest_func_t;
    static  int closure_function(lua_State* ls_)
    {
        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::init_value();
        typename arg_traits_t<ARG2>::arg_type_t arg2 = init_value_traits_t<ARG2>::init_value();
        lua_traits_t<ARG1>::check_lua_param(ls_, LUA_ARG_INDEX(1), arg1);
        lua_traits_t<ARG2>::check_lua_param(ls_, LUA_ARG_INDEX(2), arg2);

        void* user_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_func_t& registed_func = *((dest_func_t*)user_data);

        RET ret = registed_func(arg1, arg2);
        lua_traits_t<RET>::push_stack(ls_, ret);
        return 1;
    }
};

template <int arg_index, typename RET, typename ARG1, typename ARG2, typename ARG3>
struct function_dispather_traits_t<boost::function<RET (ARG1, ARG2, ARG3)>, arg_index>
{
    typedef boost::function<RET (ARG1, ARG2, ARG3)> dest_func_t;
    static  int closure_function(lua_State* ls_)
    {
        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::init_value();
        typename arg_traits_t<ARG2>::arg_type_t arg2 = init_value_traits_t<ARG2>::init_value();
        typename arg_traits_t<ARG3>::arg_type_t arg3 = init_value_traits_t<ARG3>::init_value();

        lua_traits_t<ARG1>::check_lua_param(ls_, LUA_ARG_INDEX(1), arg1);
        lua_traits_t<ARG2>::check_lua_param(ls_, LUA_ARG_INDEX(2), arg2);
        lua_traits_t<ARG3>::check_lua_param(ls_, LUA_ARG_INDEX(3), arg3);

        void* user_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_func_t& registed_func = *((dest_func_t*)user_data);

        RET ret = registed_func(arg1, arg2, arg3);
        lua_traits_t<RET>::push_stack(ls_, ret);
        return 1;
    }
};

template <int arg_index, typename RET, typename ARG1, typename ARG2, typename ARG3, typename ARG4>
struct function_dispather_traits_t<boost::function<RET (ARG1, ARG2, ARG3, ARG4)>, arg_index>
{
    typedef boost::function<RET (ARG1, ARG2, ARG3, ARG4)> dest_func_t;
    static  int closure_function(lua_State* ls_)
    {
        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::init_value();
        typename arg_traits_t<ARG2>::arg_type_t arg2 = init_value_traits_t<ARG2>::init_value();
        typename arg_traits_t<ARG3>::arg_type_t arg3 = init_value_traits_t<ARG3>::init_value();
        typename arg_traits_t<ARG4>::arg_type_t arg4 = init_value_traits_t<ARG4>::init_value();

        lua_traits_t<ARG1>::check_lua_param(ls_, LUA_ARG_INDEX(1), arg1);
        lua_traits_t<ARG2>::check_lua_param(ls_, LUA_ARG_INDEX(2), arg2);
        lua_traits_t<ARG3>::check_lua_param(ls_, LUA_ARG_INDEX(3), arg3);
        lua_traits_t<ARG4>::check_lua_param(ls_, LUA_ARG_INDEX(4), arg4);

        void* user_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_func_t& registed_func = *((dest_func_t*)user_data);

        RET ret = registed_func(arg1, arg2, arg3, arg4);
        lua_traits_t<RET>::push_stack(ls_, ret);
        return 1;
    }
};

template <int arg_index, typename RET, typename ARG1, typename ARG2, typename ARG3, typename ARG4,
          typename ARG5>
struct function_dispather_traits_t<boost::function<RET (ARG1, ARG2, ARG3, ARG4, ARG5)>, arg_index>
{
    typedef boost::function<RET (ARG1, ARG2, ARG3, ARG4, ARG5)> dest_func_t;
    static  int closure_function(lua_State* ls_)
    {
        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::init_value();
        typename arg_traits_t<ARG2>::arg_type_t arg2 = init_value_traits_t<ARG2>::init_value();
        typename arg_traits_t<ARG3>::arg_type_t arg3 = init_value_traits_t<ARG3>::init_value();
        typename arg_traits_t<ARG4>::arg_type_t arg4 = init_value_traits_t<ARG4>::init_value();
        typename arg_traits_t<ARG5>::arg_type_t arg5 = init_value_traits_t<ARG5>::init_value();

        lua_traits_t<ARG1>::check_lua_param(ls_, LUA_ARG_INDEX(1), arg1);
        lua_traits_t<ARG2>::check_lua_param(ls_, LUA_ARG_INDEX(2), arg2);
        lua_traits_t<ARG3>::check_lua_param(ls_, LUA_ARG_INDEX(3), arg3);
        lua_traits_t<ARG4>::check_lua_param(ls_, LUA_ARG_INDEX(4), arg4);
        lua_traits_t<ARG5>::check_lua_param(ls_, LUA_ARG_INDEX(5), arg5);

        void* user_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_func_t& registed_func = *((dest_func_t*)user_data);

        RET ret = registed_func(arg1, arg2, arg3, arg4, arg5);
        lua_traits_t<RET>::push_stack(ls_, ret);
        return 1;
    }
};


template <int arg_index, typename RET, typename ARG1, typename ARG2, typename ARG3, typename ARG4,
          typename ARG5, typename ARG6>
struct function_dispather_traits_t<boost::function<RET (ARG1, ARG2, ARG3, ARG4, ARG5, ARG6)>, arg_index>
{
    typedef boost::function<RET (ARG1, ARG2, ARG3, ARG4, ARG5, ARG6)> dest_func_t;
    static  int closure_function(lua_State* ls_)
    {
        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::init_value();
        typename arg_traits_t<ARG2>::arg_type_t arg2 = init_value_traits_t<ARG2>::init_value();
        typename arg_traits_t<ARG3>::arg_type_t arg3 = init_value_traits_t<ARG3>::init_value();
        typename arg_traits_t<ARG4>::arg_type_t arg4 = init_value_traits_t<ARG4>::init_value();
        typename arg_traits_t<ARG5>::arg_type_t arg5 = init_value_traits_t<ARG5>::init_value();
        typename arg_traits_t<ARG6>::arg_type_t arg6 = init_value_traits_t<ARG6>::init_value();

        lua_traits_t<ARG1>::check_lua_param(ls_, LUA_ARG_INDEX(1), arg1);
        lua_traits_t<ARG2>::check_lua_param(ls_, LUA_ARG_INDEX(2), arg2);
        lua_traits_t<ARG3>::check_lua_param(ls_, LUA_ARG_INDEX(3), arg3);
        lua_traits_t<ARG4>::check_lua_param(ls_, LUA_ARG_INDEX(4), arg4);
        lua_traits_t<ARG5>::check_lua_param(ls_, LUA_ARG_INDEX(5), arg5);
        lua_traits_t<ARG6>::check_lua_param(ls_, LUA_ARG_INDEX(6), arg6);

        void* user_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_func_t& registed_func = *((dest_func_t*)user_data);

        RET ret = registed_func(arg1, arg2, arg3, arg4, arg5, arg6);
        lua_traits_t<RET>::push_stack(ls_, ret);
        return 1;
    }
};

template <int arg_index, typename RET, typename ARG1, typename ARG2, typename ARG3, typename ARG4,
          typename ARG5, typename ARG6, typename ARG7>
struct function_dispather_traits_t<boost::function<RET (ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7)>, arg_index>
{
    typedef boost::function<RET (ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7)> dest_func_t;
    static  int closure_function(lua_State* ls_)
    {
        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::init_value();
        typename arg_traits_t<ARG2>::arg_type_t arg2 = init_value_traits_t<ARG2>::init_value();
        typename arg_traits_t<ARG3>::arg_type_t arg3 = init_value_traits_t<ARG3>::init_value();
        typename arg_traits_t<ARG4>::arg_type_t arg4 = init_value_traits_t<ARG4>::init_value();
        typename arg_traits_t<ARG5>::arg_type_t arg5 = init_value_traits_t<ARG5>::init_value();
        typename arg_traits_t<ARG6>::arg_type_t arg6 = init_value_traits_t<ARG6>::init_value();
        typename arg_traits_t<ARG7>::arg_type_t arg7 = init_value_traits_t<ARG7>::init_value();

        lua_traits_t<ARG1>::check_lua_param(ls_, LUA_ARG_INDEX(1), arg1);
        lua_traits_t<ARG2>::check_lua_param(ls_, LUA_ARG_INDEX(2), arg2);
        lua_traits_t<ARG3>::check_lua_param(ls_, LUA_ARG_INDEX(3), arg3);
        lua_traits_t<ARG4>::check_lua_param(ls_, LUA_ARG_INDEX(4), arg4);
        lua_traits_t<ARG5>::check_lua_param(ls_, LUA_ARG_INDEX(5), arg5);
        lua_traits_t<ARG6>::check_lua_param(ls_, LUA_ARG_INDEX(6), arg6);
        lua_traits_t<ARG7>::check_lua_param(ls_, LUA_ARG_INDEX(7), arg7);

        void* user_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_func_t& registed_func = *((dest_func_t*)user_data);

        RET ret = registed_func(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
        lua_traits_t<RET>::push_stack(ls_, ret);
        return 1;
    }
};

template <int arg_index, typename RET, typename ARG1, typename ARG2, typename ARG3, typename ARG4,
          typename ARG5, typename ARG6, typename ARG7, typename ARG8>
struct function_dispather_traits_t<boost::function<RET (ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8)>, arg_index>
{
    typedef boost::function<RET (ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8)> dest_func_t;
    static  int closure_function(lua_State* ls_)
    {
        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::init_value();
        typename arg_traits_t<ARG2>::arg_type_t arg2 = init_value_traits_t<ARG2>::init_value();
        typename arg_traits_t<ARG3>::arg_type_t arg3 = init_value_traits_t<ARG3>::init_value();
        typename arg_traits_t<ARG4>::arg_type_t arg4 = init_value_traits_t<ARG4>::init_value();
        typename arg_traits_t<ARG5>::arg_type_t arg5 = init_value_traits_t<ARG5>::init_value();
        typename arg_traits_t<ARG6>::arg_type_t arg6 = init_value_traits_t<ARG6>::init_value();
        typename arg_traits_t<ARG7>::arg_type_t arg7 = init_value_traits_t<ARG7>::init_value();
        typename arg_traits_t<ARG8>::arg_type_t arg8 = init_value_traits_t<ARG8>::init_value();

        lua_traits_t<ARG1>::check_lua_param(ls_, LUA_ARG_INDEX(1), arg1);
        lua_traits_t<ARG2>::check_lua_param(ls_, LUA_ARG_INDEX(2), arg2);
        lua_traits_t<ARG3>::check_lua_param(ls_, LUA_ARG_INDEX(3), arg3);
        lua_traits_t<ARG4>::check_lua_param(ls_, LUA_ARG_INDEX(4), arg4);
        lua_traits_t<ARG5>::check_lua_param(ls_, LUA_ARG_INDEX(5), arg5);
        lua_traits_t<ARG6>::check_lua_param(ls_, LUA_ARG_INDEX(6), arg6);
        lua_traits_t<ARG7>::check_lua_param(ls_, LUA_ARG_INDEX(7), arg7);
        lua_traits_t<ARG8>::check_lua_param(ls_, LUA_ARG_INDEX(8), arg8);

        void* user_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_func_t& registed_func = *((dest_func_t*)user_data);

        RET ret = registed_func(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
        lua_traits_t<RET>::push_stack(ls_, ret);
        return 1;
    }
};

template <int arg_index, typename RET, typename ARG1, typename ARG2, typename ARG3, typename ARG4,
          typename ARG5, typename ARG6, typename ARG7, typename ARG8, typename ARG9>
struct function_dispather_traits_t<boost::function<RET (ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8, ARG9)>, arg_index>
{
    typedef boost::function<RET (ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8, ARG9)> dest_func_t;
    static  int closure_function(lua_State* ls_)
    {
        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::init_value();
        typename arg_traits_t<ARG2>::arg_type_t arg2 = init_value_traits_t<ARG2>::init_value();
        typename arg_traits_t<ARG3>::arg_type_t arg3 = init_value_traits_t<ARG3>::init_value();
        typename arg_traits_t<ARG4>::arg_type_t arg4 = init_value_traits_t<ARG4>::init_value();
        typename arg_traits_t<ARG5>::arg_type_t arg5 = init_value_traits_t<ARG5>::init_value();
        typename arg_traits_t<ARG6>::arg_type_t arg6 = init_value_traits_t<ARG6>::init_value();
        typename arg_traits_t<ARG7>::arg_type_t arg7 = init_value_traits_t<ARG7>::init_value();
        typename arg_traits_t<ARG8>::arg_type_t arg8 = init_value_traits_t<ARG8>::init_value();
        typename arg_traits_t<ARG9>::arg_type_t arg9 = init_value_traits_t<ARG9>::init_value();

        lua_traits_t<ARG1>::check_lua_param(ls_, LUA_ARG_INDEX(1), arg1);
        lua_traits_t<ARG2>::check_lua_param(ls_, LUA_ARG_INDEX(2), arg2);
        lua_traits_t<ARG3>::check_lua_param(ls_, LUA_ARG_INDEX(3), arg3);
        lua_traits_t<ARG4>::check_lua_param(ls_, LUA_ARG_INDEX(4), arg4);
        lua_traits_t<ARG5>::check_lua_param(ls_, LUA_ARG_INDEX(5), arg5);
        lua_traits_t<ARG6>::check_lua_param(ls_, LUA_ARG_INDEX(6), arg6);
        lua_traits_t<ARG7>::check_lua_param(ls_, LUA_ARG_INDEX(7), arg7);
        lua_traits_t<ARG8>::check_lua_param(ls_, LUA_ARG_INDEX(8), arg8);
        lua_traits_t<ARG9>::check_lua_param(ls_, LUA_ARG_INDEX(9), arg9);

        void* user_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_func_t& registed_func = *((dest_func_t*)user_data);

        RET ret = registed_func(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
        lua_traits_t<RET>::push_stack(ls_, ret);
        return 1;
    }
};
