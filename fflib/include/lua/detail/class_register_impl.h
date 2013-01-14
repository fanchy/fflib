#ifndef _CLASS_REGISTER_IMPL_H_
#define _CLASS_REGISTER_IMPL_H_

#include <lua.hpp>
#include <string>
using namespace std;

#include <boost/function.hpp>
#include "lua_object.h"
#include "lua_traits.h"

#define ARG_INDEX(x) (x)

template<typename FUNC_TYPE>
struct register_data_t
{
    register_data_t(FUNC_TYPE func_):
        dest_func(func_)
    {
    }

    FUNC_TYPE   dest_func;
};

template<typename OBJECT>
static OBJECT* check_userdata_type(lua_State* ls_)
{
    if (false == lua_type_registry_t<OBJECT>::is_registed())
    {
        luaL_argerror(ls_, 1, "arg 1 can't convert to class*, because the class has not registed to Lua");
    }

    void* arg_data = luaL_checkudata(ls_, 1, lua_type_registry_t<OBJECT>::get_type_name());

    if (NULL == arg_data)
    {
        char buff[512];
        snprintf(buff, sizeof(buff), "`%s` expect arg 1, but arg == null",
                                     lua_type_registry_t<OBJECT>::get_type_name());
        luaL_argerror(ls_, 1, buff);
    }

    OBJECT* ret_ptr = ((class_userdata_t<OBJECT>*)arg_data)->obj_ptr;
    if (NULL == ret_ptr)
    {
        char buff[512];
        snprintf(buff, sizeof(buff), "`%s` object ptr can't be null",
                                     lua_type_registry_t<OBJECT>::get_type_name());
        luaL_argerror(ls_, 1, buff);
    }
    return ret_ptr;
}

template<typename OBJECT>
static OBJECT** get_userdata_ref(lua_State* ls_)
{
    if (false == lua_type_registry_t<OBJECT>::is_registed())
    {
        luaL_argerror(ls_, 1, "arg 1 can't convert to class*, because the class has not registed to Lua");
    }

    void* arg_data = luaL_checkudata(ls_, 1, lua_type_registry_t<OBJECT>::get_type_name());

    if (NULL == arg_data)
    {
        char buff[512];
        snprintf(buff, sizeof(buff), "`%s` expect arg 1, but arg == null",
                                     lua_type_registry_t<OBJECT>::get_type_name());
        luaL_argerror(ls_, 1, buff);
    }

    OBJECT** ret_ptr = &(((class_userdata_t<OBJECT>*)arg_data)->obj_ptr);
    return ret_ptr;
}
template <typename CLASS_TYPE, typename FUNC_TYPE>
class class_register_impl_t;

//! -------------------------------------------------------- impl code for deconstructor
template <typename CLASS_TYPE>
struct class_register_impl_t<lua_class_delete_t, CLASS_TYPE>
{
    typedef void(*dest_func_t)();
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int closure_function(lua_State* ls_)
    {
        //! 1. get closuser userdata. metatable(or typename) store in userdata.
        //! 2. check arg valid.
        //! 3. deconstruct class instance

        CLASS_TYPE** obj_ptr = get_userdata_ref<CLASS_TYPE>(ls_);

        //! obj_ptr->CLASS_TYPE::~CLASS_TYPE();
        delete *obj_ptr;
        *obj_ptr = NULL;
        return 0;
    }
};

//! -------------------------------------------------------- impl code for constructor
template <typename CLASS_TYPE, typename RET>
struct class_register_impl_t<lua_class_instance_t, pair<CLASS_TYPE, RET()> >
{
    typedef RET(*dest_func_t)();
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int closure_function(lua_State* ls_)
    {
        //! 1. get closuser userdata. metatable(or typename) store in userdata.
        //! 2. check arg valid.
        //! 3. construct class instance, set the metatable
        //! 4. return userdata to lua.

        void* user_data_ptr = lua_newuserdata(ls_, sizeof(class_userdata_t<CLASS_TYPE>));
        luaL_getmetatable(ls_, lua_type_registry_t<CLASS_TYPE>::get_type_name());
        lua_setmetatable(ls_, -2);

        new(user_data_ptr) class_userdata_t<CLASS_TYPE>(new CLASS_TYPE());
        return 1;
    }
};

template <typename CLASS_TYPE, typename RET, typename ARG1>
struct class_register_impl_t<lua_class_instance_t, pair<CLASS_TYPE, RET(ARG1)> >
{
    typedef RET(*dest_func_t)();
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int closure_function(lua_State* ls_)
    {
        //! 1. get closuser userdata. metatable(or typename) store in userdata.
        //! 2. check arg valid.
        //! 3. construct class instance, set the metatable
        //! 4. return userdata to lua.

        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::value();
        lua_traits_t<ARG1>::check_lua_param(ls_, ARG_INDEX(2), arg1);

        void* user_data_ptr = lua_newuserdata(ls_, sizeof(class_userdata_t<CLASS_TYPE>));
        luaL_getmetatable(ls_, lua_type_registry_t<CLASS_TYPE>::get_type_name());
        lua_setmetatable(ls_, -2);

        new(user_data_ptr) class_userdata_t<CLASS_TYPE>(new CLASS_TYPE(arg1));
        return 1;
    }
};

template <typename CLASS_TYPE, typename RET, typename ARG1, typename ARG2>
struct class_register_impl_t<lua_class_instance_t, pair<CLASS_TYPE, RET(ARG1, ARG2)> >
{
    typedef RET(*dest_func_t)();
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int closure_function(lua_State* ls_)
    {
        //! 1. get closuser userdata. metatable(or typename) store in userdata.
        //! 2. check arg valid.
        //! 3. construct class instance, set the metatable
        //! 4. return userdata to lua.

        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::value();
        typename arg_traits_t<ARG2>::arg_type_t arg2 = init_value_traits_t<ARG2>::value();
        lua_traits_t<ARG1>::check_lua_param(ls_, ARG_INDEX(2), arg1);
        lua_traits_t<ARG2>::check_lua_param(ls_, ARG_INDEX(3), arg2);

        void* user_data_ptr = lua_newuserdata(ls_, sizeof(class_userdata_t<CLASS_TYPE>));
        luaL_getmetatable(ls_, lua_type_registry_t<CLASS_TYPE>::get_type_name());
        lua_setmetatable(ls_, -2);

        new(user_data_ptr) class_userdata_t<CLASS_TYPE>(new CLASS_TYPE(arg1, arg2));
        return 1;
    }
};

template <typename CLASS_TYPE, typename RET, typename ARG1, typename ARG2, typename ARG3>
struct class_register_impl_t<lua_class_instance_t, pair<CLASS_TYPE, RET(ARG1, ARG2, ARG3)> >
{
    typedef RET(*dest_func_t)();
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int closure_function(lua_State* ls_)
    {
        //! 1. get closuser userdata. metatable(or typename) store in userdata.
        //! 2. check arg valid.
        //! 3. construct class instance, set the metatable
        //! 4. return userdata to lua.

        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::value();
        typename arg_traits_t<ARG2>::arg_type_t arg2 = init_value_traits_t<ARG2>::value();
        typename arg_traits_t<ARG3>::arg_type_t arg3 = init_value_traits_t<ARG3>::value();
        lua_traits_t<ARG1>::check_lua_param(ls_, ARG_INDEX(2), arg1);
        lua_traits_t<ARG2>::check_lua_param(ls_, ARG_INDEX(3), arg2);
        lua_traits_t<ARG3>::check_lua_param(ls_, ARG_INDEX(4), arg3);

        void* user_data_ptr = lua_newuserdata(ls_, sizeof(class_userdata_t<CLASS_TYPE>));

        luaL_getmetatable(ls_, lua_type_registry_t<CLASS_TYPE>::get_type_name());
        lua_setmetatable(ls_, -2);

        new(user_data_ptr) class_userdata_t<CLASS_TYPE>(new CLASS_TYPE(arg1, arg2, arg3));
        return 1;
    }
};

template <typename CLASS_TYPE, typename RET, typename ARG1, typename ARG2, typename ARG3,
          typename ARG4>
struct class_register_impl_t<lua_class_instance_t, pair<CLASS_TYPE, RET(ARG1, ARG2, ARG3, ARG4)> >
{
    typedef RET(*dest_func_t)();
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int closure_function(lua_State* ls_)
    {
        //! 1. get closuser userdata. metatable(or typename) store in userdata.
        //! 2. check arg valid.
        //! 3. construct class instance, set the metatable
        //! 4. return userdata to lua.

        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::value();
        typename arg_traits_t<ARG2>::arg_type_t arg2 = init_value_traits_t<ARG2>::value();
        typename arg_traits_t<ARG3>::arg_type_t arg3 = init_value_traits_t<ARG3>::value();
        typename arg_traits_t<ARG4>::arg_type_t arg4 = init_value_traits_t<ARG4>::value();
        lua_traits_t<ARG1>::check_lua_param(ls_, ARG_INDEX(2), arg1);
        lua_traits_t<ARG2>::check_lua_param(ls_, ARG_INDEX(3), arg2);
        lua_traits_t<ARG3>::check_lua_param(ls_, ARG_INDEX(4), arg3);
        lua_traits_t<ARG4>::check_lua_param(ls_, ARG_INDEX(5), arg4);

        void* user_data_ptr = lua_newuserdata(ls_, sizeof(class_userdata_t<CLASS_TYPE>));
        luaL_getmetatable(ls_, lua_type_registry_t<CLASS_TYPE>::get_type_name());
        lua_setmetatable(ls_, -2);

        new(user_data_ptr) class_userdata_t<CLASS_TYPE>(new CLASS_TYPE(arg1, arg2, arg3, arg4));
        return 1;
    }
};

template <typename CLASS_TYPE, typename RET, typename ARG1, typename ARG2, typename ARG3,
          typename ARG4, typename ARG5>
struct class_register_impl_t<lua_class_instance_t, pair<CLASS_TYPE, RET(ARG1, ARG2, ARG3, ARG4,
                                               ARG5)> >
{
    typedef RET(*dest_func_t)();
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int closure_function(lua_State* ls_)
    {
        //! 1. get closuser userdata. metatable(or typename) store in userdata.
        //! 2. check arg valid.
        //! 3. construct class instance, set the metatable
        //! 4. return userdata to lua.

        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::value();
        typename arg_traits_t<ARG2>::arg_type_t arg2 = init_value_traits_t<ARG2>::value();
        typename arg_traits_t<ARG3>::arg_type_t arg3 = init_value_traits_t<ARG3>::value();
        typename arg_traits_t<ARG4>::arg_type_t arg4 = init_value_traits_t<ARG4>::value();
        typename arg_traits_t<ARG5>::arg_type_t arg5 = init_value_traits_t<ARG5>::value();
        lua_traits_t<ARG1>::check_lua_param(ls_, ARG_INDEX(2), arg1);
        lua_traits_t<ARG2>::check_lua_param(ls_, ARG_INDEX(3), arg2);
        lua_traits_t<ARG3>::check_lua_param(ls_, ARG_INDEX(4), arg3);
        lua_traits_t<ARG4>::check_lua_param(ls_, ARG_INDEX(5), arg4);
        lua_traits_t<ARG5>::check_lua_param(ls_, ARG_INDEX(6), arg5);

        void* user_data_ptr = lua_newuserdata(ls_, sizeof(class_userdata_t<CLASS_TYPE>));
        luaL_getmetatable(ls_, lua_type_registry_t<CLASS_TYPE>::get_type_name());
        lua_setmetatable(ls_, -2);

        new(user_data_ptr) class_userdata_t<CLASS_TYPE>(new CLASS_TYPE(arg1, arg2, arg3, arg4, arg5));
        return 1;
    }
};

template <typename CLASS_TYPE, typename RET, typename ARG1, typename ARG2, typename ARG3,
          typename ARG4, typename ARG5, typename ARG6>
struct class_register_impl_t<lua_class_instance_t, pair<CLASS_TYPE, RET(ARG1, ARG2, ARG3,
                                               ARG4, ARG5, ARG6)> >
{
    typedef RET(*dest_func_t)();
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int closure_function(lua_State* ls_)
    {
        //! 1. get closuser userdata. metatable(or typename) store in userdata.
        //! 2. check arg valid.
        //! 3. construct class instance, set the metatable
        //! 4. return userdata to lua.

        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::value();
        typename arg_traits_t<ARG2>::arg_type_t arg2 = init_value_traits_t<ARG2>::value();
        typename arg_traits_t<ARG3>::arg_type_t arg3 = init_value_traits_t<ARG3>::value();
        typename arg_traits_t<ARG4>::arg_type_t arg4 = init_value_traits_t<ARG4>::value();
        typename arg_traits_t<ARG5>::arg_type_t arg5 = init_value_traits_t<ARG5>::value();
        typename arg_traits_t<ARG6>::arg_type_t arg6 = init_value_traits_t<ARG6>::value();
        lua_traits_t<ARG1>::check_lua_param(ls_, ARG_INDEX(2), arg1);
        lua_traits_t<ARG2>::check_lua_param(ls_, ARG_INDEX(3), arg2);
        lua_traits_t<ARG3>::check_lua_param(ls_, ARG_INDEX(4), arg3);
        lua_traits_t<ARG4>::check_lua_param(ls_, ARG_INDEX(5), arg4);
        lua_traits_t<ARG5>::check_lua_param(ls_, ARG_INDEX(6), arg5);
        lua_traits_t<ARG6>::check_lua_param(ls_, ARG_INDEX(7), arg6);

        void* user_data_ptr = lua_newuserdata(ls_, sizeof(class_userdata_t<CLASS_TYPE>));
        luaL_getmetatable(ls_, lua_type_registry_t<CLASS_TYPE>::get_type_name());
        lua_setmetatable(ls_, -2);

        new(user_data_ptr) class_userdata_t<CLASS_TYPE>(new CLASS_TYPE(arg1, arg2, arg3, arg4, arg5, arg6));
        return 1;
    }
};

template <typename CLASS_TYPE, typename RET, typename ARG1, typename ARG2, typename ARG3,
          typename ARG4, typename ARG5, typename ARG6, typename ARG7>
struct class_register_impl_t<lua_class_instance_t, pair<CLASS_TYPE, RET(ARG1, ARG2, ARG3,
                                               ARG4, ARG5, ARG6, ARG7)> >
{
    typedef RET(*dest_func_t)();
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int closure_function(lua_State* ls_)
    {
        //! 1. get closuser userdata. metatable(or typename) store in userdata.
        //! 2. check arg valid.
        //! 3. construct class instance, set the metatable
        //! 4. return userdata to lua.

        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::value();
        typename arg_traits_t<ARG2>::arg_type_t arg2 = init_value_traits_t<ARG2>::value();
        typename arg_traits_t<ARG3>::arg_type_t arg3 = init_value_traits_t<ARG3>::value();
        typename arg_traits_t<ARG4>::arg_type_t arg4 = init_value_traits_t<ARG4>::value();
        typename arg_traits_t<ARG5>::arg_type_t arg5 = init_value_traits_t<ARG5>::value();
        typename arg_traits_t<ARG6>::arg_type_t arg6 = init_value_traits_t<ARG6>::value();
        typename arg_traits_t<ARG7>::arg_type_t arg7 = init_value_traits_t<ARG7>::value();
        lua_traits_t<ARG1>::check_lua_param(ls_, ARG_INDEX(2), arg1);
        lua_traits_t<ARG2>::check_lua_param(ls_, ARG_INDEX(3), arg2);
        lua_traits_t<ARG3>::check_lua_param(ls_, ARG_INDEX(4), arg3);
        lua_traits_t<ARG4>::check_lua_param(ls_, ARG_INDEX(5), arg4);
        lua_traits_t<ARG5>::check_lua_param(ls_, ARG_INDEX(6), arg5);
        lua_traits_t<ARG6>::check_lua_param(ls_, ARG_INDEX(7), arg6);
        lua_traits_t<ARG7>::check_lua_param(ls_, ARG_INDEX(8), arg7);

        void* user_data_ptr = lua_newuserdata(ls_, sizeof(class_userdata_t<CLASS_TYPE>));

        luaL_getmetatable(ls_, lua_type_registry_t<CLASS_TYPE>::get_type_name());
        lua_setmetatable(ls_, -2);

        new(user_data_ptr) class_userdata_t<CLASS_TYPE>(new CLASS_TYPE(arg1, arg2, arg3, arg4, arg5,
                                                        arg6, arg7));
        return 1;
    }
};

template <typename CLASS_TYPE, typename RET, typename ARG1, typename ARG2, typename ARG3,
          typename ARG4, typename ARG5, typename ARG6, typename ARG7, typename ARG8>
struct class_register_impl_t<lua_class_instance_t, pair<CLASS_TYPE, RET(ARG1, ARG2, ARG3,
                                               ARG4, ARG5, ARG6, ARG7, ARG8)> >
{
    typedef RET(*dest_func_t)();
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int closure_function(lua_State* ls_)
    {
        //! 1. get closuser userdata. metatable(or typename) store in userdata.
        //! 2. check arg valid.
        //! 3. construct class instance, set the metatable
        //! 4. return userdata to lua.

        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::value();
        typename arg_traits_t<ARG2>::arg_type_t arg2 = init_value_traits_t<ARG2>::value();
        typename arg_traits_t<ARG3>::arg_type_t arg3 = init_value_traits_t<ARG3>::value();
        typename arg_traits_t<ARG4>::arg_type_t arg4 = init_value_traits_t<ARG4>::value();
        typename arg_traits_t<ARG5>::arg_type_t arg5 = init_value_traits_t<ARG5>::value();
        typename arg_traits_t<ARG6>::arg_type_t arg6 = init_value_traits_t<ARG6>::value();
        typename arg_traits_t<ARG7>::arg_type_t arg7 = init_value_traits_t<ARG7>::value();
        typename arg_traits_t<ARG8>::arg_type_t arg8 = init_value_traits_t<ARG8>::value();
        lua_traits_t<ARG1>::check_lua_param(ls_, ARG_INDEX(2), arg1);
        lua_traits_t<ARG2>::check_lua_param(ls_, ARG_INDEX(3), arg2);
        lua_traits_t<ARG3>::check_lua_param(ls_, ARG_INDEX(4), arg3);
        lua_traits_t<ARG4>::check_lua_param(ls_, ARG_INDEX(5), arg4);
        lua_traits_t<ARG5>::check_lua_param(ls_, ARG_INDEX(6), arg5);
        lua_traits_t<ARG6>::check_lua_param(ls_, ARG_INDEX(7), arg6);
        lua_traits_t<ARG7>::check_lua_param(ls_, ARG_INDEX(8), arg7);
        lua_traits_t<ARG8>::check_lua_param(ls_, ARG_INDEX(9), arg8);

        void* user_data_ptr = lua_newuserdata(ls_, sizeof(class_userdata_t<CLASS_TYPE>));
        luaL_getmetatable(ls_, lua_type_registry_t<CLASS_TYPE>::get_type_name());
        lua_setmetatable(ls_, -2);

        new(user_data_ptr) class_userdata_t<CLASS_TYPE>(new CLASS_TYPE(arg1, arg2, arg3, arg4, arg5, arg6,
                                                        arg7, arg8));
        return 1;
    }
};

template <typename CLASS_TYPE, typename RET, typename ARG1, typename ARG2, typename ARG3,
          typename ARG4, typename ARG5, typename ARG6, typename ARG7, typename ARG8,
          typename ARG9>
struct class_register_impl_t<lua_class_instance_t, pair<CLASS_TYPE, RET(ARG1, ARG2, ARG3,
                                               ARG4, ARG5, ARG6, ARG7, ARG8, ARG9)> >
{
    typedef RET(*dest_func_t)();
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int closure_function(lua_State* ls_)
    {
        //! 1. get closuser userdata. metatable(or typename) store in userdata.
        //! 2. check arg valid.
        //! 3. construct class instance, set the metatable
        //! 4. return userdata to lua.

        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::value();
        typename arg_traits_t<ARG2>::arg_type_t arg2 = init_value_traits_t<ARG2>::value();
        typename arg_traits_t<ARG3>::arg_type_t arg3 = init_value_traits_t<ARG3>::value();
        typename arg_traits_t<ARG4>::arg_type_t arg4 = init_value_traits_t<ARG4>::value();
        typename arg_traits_t<ARG5>::arg_type_t arg5 = init_value_traits_t<ARG5>::value();
        typename arg_traits_t<ARG6>::arg_type_t arg6 = init_value_traits_t<ARG6>::value();
        typename arg_traits_t<ARG7>::arg_type_t arg7 = init_value_traits_t<ARG7>::value();
        typename arg_traits_t<ARG8>::arg_type_t arg8 = init_value_traits_t<ARG8>::value();
        typename arg_traits_t<ARG9>::arg_type_t arg9 = init_value_traits_t<ARG9>::value();
        lua_traits_t<ARG1>::check_lua_param(ls_, ARG_INDEX(2), arg1);
        lua_traits_t<ARG2>::check_lua_param(ls_, ARG_INDEX(3), arg2);
        lua_traits_t<ARG3>::check_lua_param(ls_, ARG_INDEX(4), arg3);
        lua_traits_t<ARG4>::check_lua_param(ls_, ARG_INDEX(5), arg4);
        lua_traits_t<ARG5>::check_lua_param(ls_, ARG_INDEX(6), arg5);
        lua_traits_t<ARG6>::check_lua_param(ls_, ARG_INDEX(7), arg6);
        lua_traits_t<ARG7>::check_lua_param(ls_, ARG_INDEX(8), arg7);
        lua_traits_t<ARG8>::check_lua_param(ls_, ARG_INDEX(9), arg8);
        lua_traits_t<ARG9>::check_lua_param(ls_, ARG_INDEX(10), arg9);

        void* user_data_ptr = lua_newuserdata(ls_, sizeof(class_userdata_t<CLASS_TYPE>));
        luaL_getmetatable(ls_, lua_type_registry_t<CLASS_TYPE>::get_type_name());
        lua_setmetatable(ls_, -2);

        new(user_data_ptr) class_userdata_t<CLASS_TYPE>(new CLASS_TYPE(arg1, arg2, arg3, arg4, arg5, arg6,
                                                        arg7, arg8, arg9));

        return 1;
    }
};

//! --------------------------------------------------------------- impl code for class member function for return void
template <typename CLASS_TYPE, typename FUNC_CLASS_TYPE>
struct class_register_impl_t<CLASS_TYPE, void (FUNC_CLASS_TYPE::*)()>
{
    typedef void (FUNC_CLASS_TYPE::*dest_func_t)();
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int closure_function(lua_State* ls_)
    {
        //! 1. check user data, if arg type check failed, reutrn arg error info to lua.
        //! 2. get the class dest, func, exe it.

        void* dest_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_register_data_t& registed_data = *((dest_register_data_t*)dest_data);

        CLASS_TYPE* obj_ptr = check_userdata_type<CLASS_TYPE>(ls_);
        (obj_ptr->*(registed_data.dest_func))();
        return 0;
    }
};

template <typename CLASS_TYPE, typename FUNC_CLASS_TYPE, typename ARG1>
struct class_register_impl_t<CLASS_TYPE, void (FUNC_CLASS_TYPE::*)(ARG1)>
{
    typedef void (FUNC_CLASS_TYPE::*dest_func_t)(ARG1);
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int closure_function(lua_State* ls_)
    {
        //! 1. check user data, if arg type check failed, reutrn arg error info to lua.
        //! 2. get the class dest, func, exe it.

        void* dest_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_register_data_t& registed_data = *((dest_register_data_t*)dest_data);

        CLASS_TYPE* obj_ptr = check_userdata_type<CLASS_TYPE>(ls_);

        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::value();
        lua_traits_t<ARG1>::check_lua_param(ls_, ARG_INDEX(2), arg1);

        (obj_ptr->*(registed_data.dest_func))(arg1);
        return 0;
    }
};

template <typename CLASS_TYPE, typename FUNC_CLASS_TYPE, typename ARG1, typename ARG2>
struct class_register_impl_t<CLASS_TYPE, void (FUNC_CLASS_TYPE::*)(ARG1, ARG2)>
{
    typedef void (FUNC_CLASS_TYPE::*dest_func_t)(ARG1, ARG2);
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int closure_function(lua_State* ls_)
    {
        //! 1. check user data, if arg type check failed, reutrn arg error info to lua.
        //! 2. get the class dest, func, exe it.

        void* dest_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_register_data_t& registed_data = *((dest_register_data_t*)dest_data);

        CLASS_TYPE* obj_ptr = check_userdata_type<CLASS_TYPE>(ls_);

        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::value();
        typename arg_traits_t<ARG2>::arg_type_t arg2 = init_value_traits_t<ARG2>::value();
        lua_traits_t<ARG1>::check_lua_param(ls_, ARG_INDEX(2), arg1);
        lua_traits_t<ARG2>::check_lua_param(ls_, ARG_INDEX(3), arg2);

        (obj_ptr->*(registed_data.dest_func))(arg1, arg2);
        return 0;
    }
};

template <typename CLASS_TYPE, typename FUNC_CLASS_TYPE, typename ARG1, typename ARG2, typename ARG3>
struct class_register_impl_t<CLASS_TYPE, void (FUNC_CLASS_TYPE::*)(ARG1, ARG2, ARG3)>
{
    typedef void (FUNC_CLASS_TYPE::*dest_func_t)(ARG1, ARG2, ARG3);
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int closure_function(lua_State* ls_)
    {
        //! 1. check user data, if arg type check failed, reutrn arg error info to lua.
        //! 2. get the class dest, func, exe it.

        void* dest_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_register_data_t& registed_data = *((dest_register_data_t*)dest_data);

        CLASS_TYPE* obj_ptr = check_userdata_type<CLASS_TYPE>(ls_);

        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::value();
        typename arg_traits_t<ARG2>::arg_type_t arg2 = init_value_traits_t<ARG2>::value();
        typename arg_traits_t<ARG3>::arg_type_t arg3 = init_value_traits_t<ARG3>::value();
        lua_traits_t<ARG1>::check_lua_param(ls_, ARG_INDEX(2), arg1);
        lua_traits_t<ARG2>::check_lua_param(ls_, ARG_INDEX(3), arg2);
        lua_traits_t<ARG3>::check_lua_param(ls_, ARG_INDEX(4), arg3);
        (obj_ptr->*(registed_data.dest_func))(arg1, arg2, arg3);
        return 0;
    }
};

template <typename CLASS_TYPE, typename FUNC_CLASS_TYPE, typename ARG1, typename ARG2, typename ARG3, typename ARG4>
struct class_register_impl_t<CLASS_TYPE, void (FUNC_CLASS_TYPE::*)(ARG1, ARG2, ARG3, ARG4)>
{
    typedef void (FUNC_CLASS_TYPE::*dest_func_t)(ARG1, ARG2, ARG3, ARG4);
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int closure_function(lua_State* ls_)
    {
        //! 1. check user data, if arg type check failed, reutrn arg error info to lua.
        //! 2. get the class dest, func, exe it.

        void* dest_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_register_data_t& registed_data = *((dest_register_data_t*)dest_data);

        CLASS_TYPE* obj_ptr = check_userdata_type<CLASS_TYPE>(ls_);

        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::value();
        typename arg_traits_t<ARG2>::arg_type_t arg2 = init_value_traits_t<ARG2>::value();
        typename arg_traits_t<ARG3>::arg_type_t arg3 = init_value_traits_t<ARG3>::value();
        typename arg_traits_t<ARG4>::arg_type_t arg4 = init_value_traits_t<ARG4>::value();
        lua_traits_t<ARG1>::check_lua_param(ls_, ARG_INDEX(2), arg1);
        lua_traits_t<ARG2>::check_lua_param(ls_, ARG_INDEX(3), arg2);
        lua_traits_t<ARG3>::check_lua_param(ls_, ARG_INDEX(4), arg3);
        lua_traits_t<ARG4>::check_lua_param(ls_, ARG_INDEX(5), arg4);

        (obj_ptr->*(registed_data.dest_func))(arg1, arg2, arg3, arg4);
        return 0;
    }
};

template <typename CLASS_TYPE, typename FUNC_CLASS_TYPE, typename ARG1, typename ARG2, typename ARG3, typename ARG4,
          typename ARG5>
struct class_register_impl_t<CLASS_TYPE, void (FUNC_CLASS_TYPE::*)(ARG1, ARG2, ARG3, ARG4, ARG5)>
{
    typedef void (FUNC_CLASS_TYPE::*dest_func_t)(ARG1, ARG2, ARG3, ARG4, ARG5);
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int closure_function(lua_State* ls_)
    {
        //! 1. check user data, if arg type check failed, reutrn arg error info to lua.
        //! 2. get the class dest, func, exe it.

        void* dest_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_register_data_t& registed_data = *((dest_register_data_t*)dest_data);

        CLASS_TYPE* obj_ptr = check_userdata_type<CLASS_TYPE>(ls_);

        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::value();
        typename arg_traits_t<ARG2>::arg_type_t arg2 = init_value_traits_t<ARG2>::value();
        typename arg_traits_t<ARG3>::arg_type_t arg3 = init_value_traits_t<ARG3>::value();
        typename arg_traits_t<ARG4>::arg_type_t arg4 = init_value_traits_t<ARG4>::value();
        typename arg_traits_t<ARG5>::arg_type_t arg5 = init_value_traits_t<ARG5>::value();

        lua_traits_t<ARG1>::check_lua_param(ls_, ARG_INDEX(2), arg1);
        lua_traits_t<ARG2>::check_lua_param(ls_, ARG_INDEX(3), arg2);
        lua_traits_t<ARG3>::check_lua_param(ls_, ARG_INDEX(4), arg3);
        lua_traits_t<ARG4>::check_lua_param(ls_, ARG_INDEX(5), arg4);
        lua_traits_t<ARG5>::check_lua_param(ls_, ARG_INDEX(6), arg5);

        (obj_ptr->*(registed_data.dest_func))(arg1, arg2, arg3, arg4, arg5);
        return 0;
    }
};


template <typename CLASS_TYPE, typename FUNC_CLASS_TYPE, typename ARG1, typename ARG2, typename ARG3, typename ARG4,
          typename ARG5, typename ARG6>
struct class_register_impl_t<CLASS_TYPE, void (FUNC_CLASS_TYPE::*)(ARG1, ARG2, ARG3, ARG4, ARG5,
                                                              ARG6)>
{
    typedef void (FUNC_CLASS_TYPE::*dest_func_t)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6);
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int closure_function(lua_State* ls_)
    {
        //! 1. check user data, if arg type check failed, reutrn arg error info to lua.
        //! 2. get the class dest, func, exe it.

        void* dest_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_register_data_t& registed_data = *((dest_register_data_t*)dest_data);

        CLASS_TYPE* obj_ptr = check_userdata_type<CLASS_TYPE>(ls_);

        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::value();
        typename arg_traits_t<ARG2>::arg_type_t arg2 = init_value_traits_t<ARG2>::value();
        typename arg_traits_t<ARG3>::arg_type_t arg3 = init_value_traits_t<ARG3>::value();
        typename arg_traits_t<ARG4>::arg_type_t arg4 = init_value_traits_t<ARG4>::value();
        typename arg_traits_t<ARG5>::arg_type_t arg5 = init_value_traits_t<ARG5>::value();
        typename arg_traits_t<ARG6>::arg_type_t arg6 = init_value_traits_t<ARG6>::value();

        lua_traits_t<ARG1>::check_lua_param(ls_, ARG_INDEX(2), arg1);
        lua_traits_t<ARG2>::check_lua_param(ls_, ARG_INDEX(3), arg2);
        lua_traits_t<ARG3>::check_lua_param(ls_, ARG_INDEX(4), arg3);
        lua_traits_t<ARG4>::check_lua_param(ls_, ARG_INDEX(5), arg4);
        lua_traits_t<ARG5>::check_lua_param(ls_, ARG_INDEX(6), arg5);
        lua_traits_t<ARG6>::check_lua_param(ls_, ARG_INDEX(7), arg6);

        (obj_ptr->*(registed_data.dest_func))(arg1, arg2, arg3, arg4, arg5, arg6);
        return 0;
    }
};

template <typename CLASS_TYPE, typename FUNC_CLASS_TYPE, typename ARG1, typename ARG2, typename ARG3, typename ARG4,
          typename ARG5, typename ARG6, typename ARG7>
struct class_register_impl_t<CLASS_TYPE, void (FUNC_CLASS_TYPE::*)(ARG1, ARG2, ARG3, ARG4, ARG5,
                                                              ARG6, ARG7)>
{
    typedef void (FUNC_CLASS_TYPE::*dest_func_t)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7);
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int closure_function(lua_State* ls_)
    {
        //! 1. check user data, if arg type check failed, reutrn arg error info to lua.
        //! 2. get the class dest, func, exe it.

        void* dest_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_register_data_t& registed_data = *((dest_register_data_t*)dest_data);

        CLASS_TYPE* obj_ptr = check_userdata_type<CLASS_TYPE>(ls_);

        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::value();
        typename arg_traits_t<ARG2>::arg_type_t arg2 = init_value_traits_t<ARG2>::value();
        typename arg_traits_t<ARG3>::arg_type_t arg3 = init_value_traits_t<ARG3>::value();
        typename arg_traits_t<ARG4>::arg_type_t arg4 = init_value_traits_t<ARG4>::value();
        typename arg_traits_t<ARG5>::arg_type_t arg5 = init_value_traits_t<ARG5>::value();
        typename arg_traits_t<ARG6>::arg_type_t arg6 = init_value_traits_t<ARG6>::value();
        typename arg_traits_t<ARG7>::arg_type_t arg7 = init_value_traits_t<ARG7>::value();

        lua_traits_t<ARG1>::check_lua_param(ls_, ARG_INDEX(2), arg1);
        lua_traits_t<ARG2>::check_lua_param(ls_, ARG_INDEX(3), arg2);
        lua_traits_t<ARG3>::check_lua_param(ls_, ARG_INDEX(4), arg3);
        lua_traits_t<ARG4>::check_lua_param(ls_, ARG_INDEX(5), arg4);
        lua_traits_t<ARG5>::check_lua_param(ls_, ARG_INDEX(6), arg5);
        lua_traits_t<ARG6>::check_lua_param(ls_, ARG_INDEX(7), arg6);
        lua_traits_t<ARG7>::check_lua_param(ls_, ARG_INDEX(8), arg7);

        (obj_ptr->*(registed_data.dest_func))(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
        return 0;
    }
};

template <typename CLASS_TYPE, typename FUNC_CLASS_TYPE, typename ARG1, typename ARG2, typename ARG3, typename ARG4,
          typename ARG5, typename ARG6, typename ARG7, typename ARG8>
struct class_register_impl_t<CLASS_TYPE, void (FUNC_CLASS_TYPE::*)(ARG1, ARG2, ARG3, ARG4, ARG5,
                                                                   ARG6, ARG7, ARG8)>
{
    typedef void (FUNC_CLASS_TYPE::*dest_func_t)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8);
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int closure_function(lua_State* ls_)
    {
        //! 1. check user data, if arg type check failed, reutrn arg error info to lua.
        //! 2. get the class dest, func, exe it.

        void* dest_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_register_data_t& registed_data = *((dest_register_data_t*)dest_data);

        CLASS_TYPE* obj_ptr = check_userdata_type<CLASS_TYPE>(ls_);

        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::value();
        typename arg_traits_t<ARG2>::arg_type_t arg2 = init_value_traits_t<ARG2>::value();
        typename arg_traits_t<ARG3>::arg_type_t arg3 = init_value_traits_t<ARG3>::value();
        typename arg_traits_t<ARG4>::arg_type_t arg4 = init_value_traits_t<ARG4>::value();
        typename arg_traits_t<ARG5>::arg_type_t arg5 = init_value_traits_t<ARG5>::value();
        typename arg_traits_t<ARG6>::arg_type_t arg6 = init_value_traits_t<ARG6>::value();
        typename arg_traits_t<ARG7>::arg_type_t arg7 = init_value_traits_t<ARG7>::value();
        typename arg_traits_t<ARG8>::arg_type_t arg8 = init_value_traits_t<ARG8>::value();

        lua_traits_t<ARG1>::check_lua_param(ls_, ARG_INDEX(2), arg1);
        lua_traits_t<ARG2>::check_lua_param(ls_, ARG_INDEX(3), arg2);
        lua_traits_t<ARG3>::check_lua_param(ls_, ARG_INDEX(4), arg3);
        lua_traits_t<ARG4>::check_lua_param(ls_, ARG_INDEX(5), arg4);
        lua_traits_t<ARG5>::check_lua_param(ls_, ARG_INDEX(6), arg5);
        lua_traits_t<ARG6>::check_lua_param(ls_, ARG_INDEX(7), arg6);
        lua_traits_t<ARG7>::check_lua_param(ls_, ARG_INDEX(8), arg7);
        lua_traits_t<ARG8>::check_lua_param(ls_, ARG_INDEX(9), arg8);

        (obj_ptr->*(registed_data.dest_func))(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
        return 0;
    }
};


template <typename CLASS_TYPE, typename FUNC_CLASS_TYPE, typename ARG1, typename ARG2, typename ARG3, typename ARG4,
          typename ARG5, typename ARG6, typename ARG7, typename ARG8, typename ARG9>
struct class_register_impl_t<CLASS_TYPE, void (FUNC_CLASS_TYPE::*)(ARG1, ARG2, ARG3, ARG4, ARG5,
                                                              ARG6, ARG7, ARG8, ARG9)>
{
    typedef void (FUNC_CLASS_TYPE::*dest_func_t)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8, ARG9);
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int closure_function(lua_State* ls_)
    {
        //! 1. check user data, if arg type check failed, reutrn arg error info to lua.
        //! 2. get the class dest, func, exe it.

        void* dest_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_register_data_t& registed_data = *((dest_register_data_t*)dest_data);

        CLASS_TYPE* obj_ptr = check_userdata_type<CLASS_TYPE>(ls_);

        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::value();
        typename arg_traits_t<ARG2>::arg_type_t arg2 = init_value_traits_t<ARG2>::value();
        typename arg_traits_t<ARG3>::arg_type_t arg3 = init_value_traits_t<ARG3>::value();
        typename arg_traits_t<ARG4>::arg_type_t arg4 = init_value_traits_t<ARG4>::value();
        typename arg_traits_t<ARG5>::arg_type_t arg5 = init_value_traits_t<ARG5>::value();
        typename arg_traits_t<ARG6>::arg_type_t arg6 = init_value_traits_t<ARG6>::value();
        typename arg_traits_t<ARG7>::arg_type_t arg7 = init_value_traits_t<ARG7>::value();
        typename arg_traits_t<ARG8>::arg_type_t arg8 = init_value_traits_t<ARG8>::value();
        typename arg_traits_t<ARG9>::arg_type_t arg9 = init_value_traits_t<ARG9>::value();

        lua_traits_t<ARG1>::check_lua_param(ls_, ARG_INDEX(2), arg1);
        lua_traits_t<ARG2>::check_lua_param(ls_, ARG_INDEX(3), arg2);
        lua_traits_t<ARG3>::check_lua_param(ls_, ARG_INDEX(4), arg3);
        lua_traits_t<ARG4>::check_lua_param(ls_, ARG_INDEX(5), arg4);
        lua_traits_t<ARG5>::check_lua_param(ls_, ARG_INDEX(6), arg5);
        lua_traits_t<ARG6>::check_lua_param(ls_, ARG_INDEX(7), arg6);
        lua_traits_t<ARG7>::check_lua_param(ls_, ARG_INDEX(8), arg7);
        lua_traits_t<ARG8>::check_lua_param(ls_, ARG_INDEX(9), arg8);
        lua_traits_t<ARG9>::check_lua_param(ls_, ARG_INDEX(10), arg9);

        (obj_ptr->*(registed_data.dest_func))(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
        return 0;
    }
};

//! --------------------------------------------------------------- impl code for class member function for return RET
template <typename CLASS_TYPE, typename FUNC_CLASS_TYPE, typename RET>
struct class_register_impl_t<CLASS_TYPE, RET (FUNC_CLASS_TYPE::*)()>
{
    typedef RET (FUNC_CLASS_TYPE::*dest_func_t)();
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int closure_function(lua_State* ls_)
    {
        //! 1. check user data, if arg type check failed, reutrn arg error info to lua.
        //! 2. get the class dest, func, exe it.

        void* dest_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_register_data_t& registed_data = *((dest_register_data_t*)dest_data);

        CLASS_TYPE* obj_ptr = check_userdata_type<CLASS_TYPE>(ls_);
        RET ret = (obj_ptr->*(registed_data.dest_func))();
        lua_traits_t<RET>::push_stack(ls_, ret);
        return 1;
    }
};

template <typename CLASS_TYPE, typename FUNC_CLASS_TYPE, typename ARG1, typename RET>
struct class_register_impl_t<CLASS_TYPE, RET (FUNC_CLASS_TYPE::*)(ARG1)>
{
    typedef RET (FUNC_CLASS_TYPE::*dest_func_t)(ARG1);
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int closure_function(lua_State* ls_)
    {
        //! 1. check user data, if arg type check failed, reutrn arg error info to lua.
        //! 2. get the class dest, func, exe it.

        void* dest_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_register_data_t& registed_data = *((dest_register_data_t*)dest_data);

        CLASS_TYPE* obj_ptr = check_userdata_type<CLASS_TYPE>(ls_);

        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::value();
        lua_traits_t<ARG1>::check_lua_param(ls_, ARG_INDEX(2), arg1);

        RET ret = (obj_ptr->*(registed_data.dest_func))(arg1);
        lua_traits_t<RET>::push_stack(ls_, ret);
        return 1;
    }
};

template <typename CLASS_TYPE, typename FUNC_CLASS_TYPE, typename ARG1, typename ARG2,
          typename RET>
struct class_register_impl_t<CLASS_TYPE, RET (FUNC_CLASS_TYPE::*)(ARG1, ARG2)>
{
    typedef RET (FUNC_CLASS_TYPE::*dest_func_t)(ARG1, ARG2);
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int closure_function(lua_State* ls_)
    {
        //! 1. check user data, if arg type check failed, reutrn arg error info to lua.
        //! 2. get the class dest, func, exe it.

        void* dest_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_register_data_t& registed_data = *((dest_register_data_t*)dest_data);

        CLASS_TYPE* obj_ptr = check_userdata_type<CLASS_TYPE>(ls_);

        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::value();
        typename arg_traits_t<ARG2>::arg_type_t arg2 = init_value_traits_t<ARG2>::value();
        lua_traits_t<ARG1>::check_lua_param(ls_, ARG_INDEX(2), arg1);
        lua_traits_t<ARG2>::check_lua_param(ls_, ARG_INDEX(3), arg2);

        RET ret = (obj_ptr->*(registed_data.dest_func))(arg1, arg2);
        lua_traits_t<RET>::push_stack(ls_, ret);
        return 1;
    }
};

template <typename CLASS_TYPE, typename FUNC_CLASS_TYPE, typename ARG1, typename ARG2, typename ARG3,
          typename RET>
struct class_register_impl_t<CLASS_TYPE, RET (FUNC_CLASS_TYPE::*)(ARG1, ARG2, ARG3)>
{
    typedef RET (FUNC_CLASS_TYPE::*dest_func_t)(ARG1, ARG2, ARG3);
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int closure_function(lua_State* ls_)
    {
        //! 1. check user data, if arg type check failed, reutrn arg error info to lua.
        //! 2. get the class dest, func, exe it.

        void* dest_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_register_data_t& registed_data = *((dest_register_data_t*)dest_data);

        CLASS_TYPE* obj_ptr = check_userdata_type<CLASS_TYPE>(ls_);

        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::value();
        typename arg_traits_t<ARG2>::arg_type_t arg2 = init_value_traits_t<ARG2>::value();
        typename arg_traits_t<ARG3>::arg_type_t arg3 = init_value_traits_t<ARG3>::value();
        lua_traits_t<ARG1>::check_lua_param(ls_, ARG_INDEX(2), arg1);
        lua_traits_t<ARG2>::check_lua_param(ls_, ARG_INDEX(3), arg2);
        lua_traits_t<ARG3>::check_lua_param(ls_, ARG_INDEX(4), arg3);

        RET ret = (obj_ptr->*(registed_data.dest_func))(arg1, arg2, arg3);
        lua_traits_t<RET>::push_stack(ls_, ret);
        return 1;
    }
};

template <typename CLASS_TYPE, typename FUNC_CLASS_TYPE, typename ARG1, typename ARG2, typename ARG3, typename ARG4,
          typename RET>
struct class_register_impl_t<CLASS_TYPE, RET (FUNC_CLASS_TYPE::*)(ARG1, ARG2, ARG3, ARG4)>
{
    typedef RET (FUNC_CLASS_TYPE::*dest_func_t)(ARG1, ARG2, ARG3, ARG4);
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int closure_function(lua_State* ls_)
    {
        //! 1. check user data, if arg type check failed, reutrn arg error info to lua.
        //! 2. get the class dest, func, exe it.

        void* dest_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_register_data_t& registed_data = *((dest_register_data_t*)dest_data);

        CLASS_TYPE* obj_ptr = check_userdata_type<CLASS_TYPE>(ls_);

        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::value();
        typename arg_traits_t<ARG2>::arg_type_t arg2 = init_value_traits_t<ARG2>::value();
        typename arg_traits_t<ARG3>::arg_type_t arg3 = init_value_traits_t<ARG3>::value();
        typename arg_traits_t<ARG4>::arg_type_t arg4 = init_value_traits_t<ARG4>::value();
        lua_traits_t<ARG1>::check_lua_param(ls_, ARG_INDEX(2), arg1);
        lua_traits_t<ARG2>::check_lua_param(ls_, ARG_INDEX(3), arg2);
        lua_traits_t<ARG3>::check_lua_param(ls_, ARG_INDEX(4), arg3);
        lua_traits_t<ARG4>::check_lua_param(ls_, ARG_INDEX(5), arg4);

        RET ret = (obj_ptr->*(registed_data.dest_func))(arg1, arg2, arg3, arg4);
        lua_traits_t<RET>::push_stack(ls_, ret);
        return 1;
    }
};

template <typename CLASS_TYPE, typename FUNC_CLASS_TYPE, typename ARG1, typename ARG2, typename ARG3, typename ARG4,
          typename ARG5, typename RET>
struct class_register_impl_t<CLASS_TYPE, RET (FUNC_CLASS_TYPE::*)(ARG1, ARG2, ARG3, ARG4, ARG5)>
{
    typedef RET (FUNC_CLASS_TYPE::*dest_func_t)(ARG1, ARG2, ARG3, ARG4, ARG5);
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int closure_function(lua_State* ls_)
    {
        //! 1. check user data, if arg type check failed, reutrn arg error info to lua.
        //! 2. get the class dest, func, exe it.

        void* dest_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_register_data_t& registed_data = *((dest_register_data_t*)dest_data);

        CLASS_TYPE* obj_ptr = check_userdata_type<CLASS_TYPE>(ls_);

        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::value();
        typename arg_traits_t<ARG2>::arg_type_t arg2 = init_value_traits_t<ARG2>::value();
        typename arg_traits_t<ARG3>::arg_type_t arg3 = init_value_traits_t<ARG3>::value();
        typename arg_traits_t<ARG4>::arg_type_t arg4 = init_value_traits_t<ARG4>::value();
        typename arg_traits_t<ARG5>::arg_type_t arg5 = init_value_traits_t<ARG5>::value();

        lua_traits_t<ARG1>::check_lua_param(ls_, ARG_INDEX(2), arg1);
        lua_traits_t<ARG2>::check_lua_param(ls_, ARG_INDEX(3), arg2);
        lua_traits_t<ARG3>::check_lua_param(ls_, ARG_INDEX(4), arg3);
        lua_traits_t<ARG4>::check_lua_param(ls_, ARG_INDEX(5), arg4);
        lua_traits_t<ARG5>::check_lua_param(ls_, ARG_INDEX(6), arg5);

        RET ret = (obj_ptr->*(registed_data.dest_func))(arg1, arg2, arg3, arg4, arg5);
        lua_traits_t<RET>::push_stack(ls_, ret);
        return 1;
    }
};


template <typename CLASS_TYPE, typename FUNC_CLASS_TYPE, typename ARG1, typename ARG2, typename ARG3, typename ARG4,
          typename ARG5, typename ARG6, typename RET>
struct class_register_impl_t<CLASS_TYPE, RET (FUNC_CLASS_TYPE::*)(ARG1, ARG2, ARG3, ARG4, ARG5,
                                                              ARG6)>
{
    typedef RET (FUNC_CLASS_TYPE::*dest_func_t)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6);
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int closure_function(lua_State* ls_)
    {
        //! 1. check user data, if arg type check failed, reutrn arg error info to lua.
        //! 2. get the class dest, func, exe it.

        void* dest_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_register_data_t& registed_data = *((dest_register_data_t*)dest_data);

        CLASS_TYPE* obj_ptr = check_userdata_type<CLASS_TYPE>(ls_);

        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::value();
        typename arg_traits_t<ARG2>::arg_type_t arg2 = init_value_traits_t<ARG2>::value();
        typename arg_traits_t<ARG3>::arg_type_t arg3 = init_value_traits_t<ARG3>::value();
        typename arg_traits_t<ARG4>::arg_type_t arg4 = init_value_traits_t<ARG4>::value();
        typename arg_traits_t<ARG5>::arg_type_t arg5 = init_value_traits_t<ARG5>::value();
        typename arg_traits_t<ARG6>::arg_type_t arg6 = init_value_traits_t<ARG6>::value();

        lua_traits_t<ARG1>::check_lua_param(ls_, ARG_INDEX(2), arg1);
        lua_traits_t<ARG2>::check_lua_param(ls_, ARG_INDEX(3), arg2);
        lua_traits_t<ARG3>::check_lua_param(ls_, ARG_INDEX(4), arg3);
        lua_traits_t<ARG4>::check_lua_param(ls_, ARG_INDEX(5), arg4);
        lua_traits_t<ARG5>::check_lua_param(ls_, ARG_INDEX(6), arg5);
        lua_traits_t<ARG6>::check_lua_param(ls_, ARG_INDEX(7), arg6);

        RET ret = (obj_ptr->*(registed_data.dest_func))(arg1, arg2, arg3, arg4, arg5, arg6);
        lua_traits_t<RET>::push_stack(ls_, ret);
        return 1;
    }
};

template <typename CLASS_TYPE, typename FUNC_CLASS_TYPE, typename ARG1, typename ARG2, typename ARG3, typename ARG4,
          typename ARG5, typename ARG6, typename ARG7, typename RET>
struct class_register_impl_t<CLASS_TYPE, RET (FUNC_CLASS_TYPE::*)(ARG1, ARG2, ARG3, ARG4, ARG5,
                                                              ARG6, ARG7)>
{
    typedef RET (FUNC_CLASS_TYPE::*dest_func_t)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7);
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int closure_function(lua_State* ls_)
    {
        //! 1. check user data, if arg type check failed, reutrn arg error info to lua.
        //! 2. get the class dest, func, exe it.

        void* dest_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_register_data_t& registed_data = *((dest_register_data_t*)dest_data);

        CLASS_TYPE* obj_ptr = check_userdata_type<CLASS_TYPE>(ls_);

        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::value();
        typename arg_traits_t<ARG2>::arg_type_t arg2 = init_value_traits_t<ARG2>::value();
        typename arg_traits_t<ARG3>::arg_type_t arg3 = init_value_traits_t<ARG3>::value();
        typename arg_traits_t<ARG4>::arg_type_t arg4 = init_value_traits_t<ARG4>::value();
        typename arg_traits_t<ARG5>::arg_type_t arg5 = init_value_traits_t<ARG5>::value();
        typename arg_traits_t<ARG6>::arg_type_t arg6 = init_value_traits_t<ARG6>::value();
        typename arg_traits_t<ARG7>::arg_type_t arg7 = init_value_traits_t<ARG7>::value();

        lua_traits_t<ARG1>::check_lua_param(ls_, ARG_INDEX(2), arg1);
        lua_traits_t<ARG2>::check_lua_param(ls_, ARG_INDEX(3), arg2);
        lua_traits_t<ARG3>::check_lua_param(ls_, ARG_INDEX(4), arg3);
        lua_traits_t<ARG4>::check_lua_param(ls_, ARG_INDEX(5), arg4);
        lua_traits_t<ARG5>::check_lua_param(ls_, ARG_INDEX(6), arg5);
        lua_traits_t<ARG6>::check_lua_param(ls_, ARG_INDEX(7), arg6);
        lua_traits_t<ARG7>::check_lua_param(ls_, ARG_INDEX(8), arg7);

        RET ret = (obj_ptr->*(registed_data.dest_func))(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
        lua_traits_t<RET>::push_stack(ls_, ret);
        return 1;
    }
};

template <typename CLASS_TYPE, typename FUNC_CLASS_TYPE, typename ARG1, typename ARG2, typename ARG3, typename ARG4,
          typename ARG5, typename ARG6, typename ARG7, typename ARG8, typename RET>
struct class_register_impl_t<CLASS_TYPE, RET (FUNC_CLASS_TYPE::*)(ARG1, ARG2, ARG3, ARG4, ARG5,
                                                                   ARG6, ARG7, ARG8)>
{
    typedef RET (FUNC_CLASS_TYPE::*dest_func_t)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8);
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int closure_function(lua_State* ls_)
    {
        //! 1. check user data, if arg type check failed, reutrn arg error info to lua.
        //! 2. get the class dest, func, exe it.

        void* dest_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_register_data_t& registed_data = *((dest_register_data_t*)dest_data);

        CLASS_TYPE* obj_ptr = check_userdata_type<CLASS_TYPE>(ls_);

        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::value();
        typename arg_traits_t<ARG2>::arg_type_t arg2 = init_value_traits_t<ARG2>::value();
        typename arg_traits_t<ARG3>::arg_type_t arg3 = init_value_traits_t<ARG3>::value();
        typename arg_traits_t<ARG4>::arg_type_t arg4 = init_value_traits_t<ARG4>::value();
        typename arg_traits_t<ARG5>::arg_type_t arg5 = init_value_traits_t<ARG5>::value();
        typename arg_traits_t<ARG6>::arg_type_t arg6 = init_value_traits_t<ARG6>::value();
        typename arg_traits_t<ARG7>::arg_type_t arg7 = init_value_traits_t<ARG7>::value();
        typename arg_traits_t<ARG8>::arg_type_t arg8 = init_value_traits_t<ARG8>::value();

        lua_traits_t<ARG1>::check_lua_param(ls_, ARG_INDEX(2), arg1);
        lua_traits_t<ARG2>::check_lua_param(ls_, ARG_INDEX(3), arg2);
        lua_traits_t<ARG3>::check_lua_param(ls_, ARG_INDEX(4), arg3);
        lua_traits_t<ARG4>::check_lua_param(ls_, ARG_INDEX(5), arg4);
        lua_traits_t<ARG5>::check_lua_param(ls_, ARG_INDEX(6), arg5);
        lua_traits_t<ARG6>::check_lua_param(ls_, ARG_INDEX(7), arg6);
        lua_traits_t<ARG7>::check_lua_param(ls_, ARG_INDEX(8), arg7);
        lua_traits_t<ARG8>::check_lua_param(ls_, ARG_INDEX(9), arg8);

        RET ret = (obj_ptr->*(registed_data.dest_func))(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
        lua_traits_t<RET>::push_stack(ls_, ret);
        return 1;
    }
};


template <typename CLASS_TYPE, typename FUNC_CLASS_TYPE, typename ARG1, typename ARG2, typename ARG3, typename ARG4,
          typename ARG5, typename ARG6, typename ARG7, typename ARG8, typename ARG9, typename RET>
struct class_register_impl_t<CLASS_TYPE, RET (FUNC_CLASS_TYPE::*)(ARG1, ARG2, ARG3, ARG4, ARG5,
                                                              ARG6, ARG7, ARG8, ARG9)>
{
    typedef RET (FUNC_CLASS_TYPE::*dest_func_t)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8, ARG9);
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int closure_function(lua_State* ls_)
    {
        //! 1. check user data, if arg type check failed, reutrn arg error info to lua.
        //! 2. get the class dest, func, exe it.

        void* dest_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_register_data_t& registed_data = *((dest_register_data_t*)dest_data);

        CLASS_TYPE* obj_ptr = check_userdata_type<CLASS_TYPE>(ls_);

        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::value();
        typename arg_traits_t<ARG2>::arg_type_t arg2 = init_value_traits_t<ARG2>::value();
        typename arg_traits_t<ARG3>::arg_type_t arg3 = init_value_traits_t<ARG3>::value();
        typename arg_traits_t<ARG4>::arg_type_t arg4 = init_value_traits_t<ARG4>::value();
        typename arg_traits_t<ARG5>::arg_type_t arg5 = init_value_traits_t<ARG5>::value();
        typename arg_traits_t<ARG6>::arg_type_t arg6 = init_value_traits_t<ARG6>::value();
        typename arg_traits_t<ARG7>::arg_type_t arg7 = init_value_traits_t<ARG7>::value();
        typename arg_traits_t<ARG8>::arg_type_t arg8 = init_value_traits_t<ARG8>::value();
        typename arg_traits_t<ARG9>::arg_type_t arg9 = init_value_traits_t<ARG9>::value();

        lua_traits_t<ARG1>::check_lua_param(ls_, ARG_INDEX(2), arg1);
        lua_traits_t<ARG2>::check_lua_param(ls_, ARG_INDEX(3), arg2);
        lua_traits_t<ARG3>::check_lua_param(ls_, ARG_INDEX(4), arg3);
        lua_traits_t<ARG4>::check_lua_param(ls_, ARG_INDEX(5), arg4);
        lua_traits_t<ARG5>::check_lua_param(ls_, ARG_INDEX(6), arg5);
        lua_traits_t<ARG6>::check_lua_param(ls_, ARG_INDEX(7), arg6);
        lua_traits_t<ARG7>::check_lua_param(ls_, ARG_INDEX(8), arg7);
        lua_traits_t<ARG8>::check_lua_param(ls_, ARG_INDEX(9), arg8);
        lua_traits_t<ARG9>::check_lua_param(ls_, ARG_INDEX(10), arg9);

        RET ret = (obj_ptr->*(registed_data.dest_func))(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
        lua_traits_t<RET>::push_stack(ls_, ret);
        return 1;
    }
};


//! ------------------------------- impl code for const class member function for return RET
template <typename CLASS_TYPE, typename FUNC_CLASS_TYPE, typename RET>
struct class_register_impl_t<CLASS_TYPE, RET (FUNC_CLASS_TYPE::*)() const>
{
    typedef RET (FUNC_CLASS_TYPE::*dest_func_t)() const;
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int closure_function(lua_State* ls_)
    {
        //! 1. check user data, if arg type check failed, reutrn arg error info to lua.
        //! 2. get the class dest, func, exe it.

        void* dest_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_register_data_t& registed_data = *((dest_register_data_t*)dest_data);

        CLASS_TYPE* obj_ptr = check_userdata_type<CLASS_TYPE>(ls_);
        RET ret = (obj_ptr->*(registed_data.dest_func))();
        lua_traits_t<RET>::push_stack(ls_, ret);
        return 1;
    }
};

template <typename CLASS_TYPE, typename FUNC_CLASS_TYPE, typename ARG1, typename RET>
struct class_register_impl_t<CLASS_TYPE, RET (FUNC_CLASS_TYPE::*)(ARG1) const>
{
    typedef RET (FUNC_CLASS_TYPE::*dest_func_t)(ARG1) const;
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int closure_function(lua_State* ls_)
    {
        //! 1. check user data, if arg type check failed, reutrn arg error info to lua.
        //! 2. get the class dest, func, exe it.

        void* dest_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_register_data_t& registed_data = *((dest_register_data_t*)dest_data);

        CLASS_TYPE* obj_ptr = check_userdata_type<CLASS_TYPE>(ls_);

        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::value();
        lua_traits_t<ARG1>::check_lua_param(ls_, ARG_INDEX(2), arg1);

        RET ret = (obj_ptr->*(registed_data.dest_func))(arg1);
        lua_traits_t<RET>::push_stack(ls_, ret);
        return 1;
    }
};

template <typename CLASS_TYPE, typename FUNC_CLASS_TYPE, typename ARG1, typename ARG2,
          typename RET>
struct class_register_impl_t<CLASS_TYPE, RET (FUNC_CLASS_TYPE::*)(ARG1, ARG2) const>
{
    typedef RET (FUNC_CLASS_TYPE::*dest_func_t)(ARG1, ARG2) const;
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int closure_function(lua_State* ls_)
    {
        //! 1. check user data, if arg type check failed, reutrn arg error info to lua.
        //! 2. get the class dest, func, exe it.

        void* dest_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_register_data_t& registed_data = *((dest_register_data_t*)dest_data);

        CLASS_TYPE* obj_ptr = check_userdata_type<CLASS_TYPE>(ls_);

        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::value();
        typename arg_traits_t<ARG2>::arg_type_t arg2 = init_value_traits_t<ARG2>::value();
        lua_traits_t<ARG1>::check_lua_param(ls_, ARG_INDEX(2), arg1);
        lua_traits_t<ARG2>::check_lua_param(ls_, ARG_INDEX(3), arg2);

        RET ret = (obj_ptr->*(registed_data.dest_func))(arg1, arg2);
        lua_traits_t<RET>::push_stack(ls_, ret);
        return 1;
    }
};

template <typename CLASS_TYPE, typename FUNC_CLASS_TYPE, typename ARG1, typename ARG2, typename ARG3,
          typename RET>
struct class_register_impl_t<CLASS_TYPE, RET (FUNC_CLASS_TYPE::*)(ARG1, ARG2, ARG3) const>
{
    typedef RET (FUNC_CLASS_TYPE::*dest_func_t)(ARG1, ARG2, ARG3) const;
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int closure_function(lua_State* ls_)
    {
        //! 1. check user data, if arg type check failed, reutrn arg error info to lua.
        //! 2. get the class dest, func, exe it.

        void* dest_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_register_data_t& registed_data = *((dest_register_data_t*)dest_data);

        CLASS_TYPE* obj_ptr = check_userdata_type<CLASS_TYPE>(ls_);

        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::value();
        typename arg_traits_t<ARG2>::arg_type_t arg2 = init_value_traits_t<ARG2>::value();
        typename arg_traits_t<ARG3>::arg_type_t arg3 = init_value_traits_t<ARG3>::value();
        lua_traits_t<ARG1>::check_lua_param(ls_, ARG_INDEX(2), arg1);
        lua_traits_t<ARG2>::check_lua_param(ls_, ARG_INDEX(3), arg2);
        lua_traits_t<ARG3>::check_lua_param(ls_, ARG_INDEX(4), arg3);

        RET ret = (obj_ptr->*(registed_data.dest_func))(arg1, arg2, arg3);
        lua_traits_t<RET>::push_stack(ls_, ret);
        return 1;
    }
};

template <typename CLASS_TYPE, typename FUNC_CLASS_TYPE, typename ARG1, typename ARG2, typename ARG3, typename ARG4,
          typename RET>
struct class_register_impl_t<CLASS_TYPE, RET (FUNC_CLASS_TYPE::*)(ARG1, ARG2, ARG3, ARG4) const>
{
    typedef RET (FUNC_CLASS_TYPE::*dest_func_t)(ARG1, ARG2, ARG3, ARG4) const;
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int closure_function(lua_State* ls_)
    {
        //! 1. check user data, if arg type check failed, reutrn arg error info to lua.
        //! 2. get the class dest, func, exe it.

        void* dest_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_register_data_t& registed_data = *((dest_register_data_t*)dest_data);

        CLASS_TYPE* obj_ptr = check_userdata_type<CLASS_TYPE>(ls_);

        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::value();
        typename arg_traits_t<ARG2>::arg_type_t arg2 = init_value_traits_t<ARG2>::value();
        typename arg_traits_t<ARG3>::arg_type_t arg3 = init_value_traits_t<ARG3>::value();
        typename arg_traits_t<ARG4>::arg_type_t arg4 = init_value_traits_t<ARG4>::value();
        lua_traits_t<ARG1>::check_lua_param(ls_, ARG_INDEX(2), arg1);
        lua_traits_t<ARG2>::check_lua_param(ls_, ARG_INDEX(3), arg2);
        lua_traits_t<ARG3>::check_lua_param(ls_, ARG_INDEX(4), arg3);
        lua_traits_t<ARG4>::check_lua_param(ls_, ARG_INDEX(5), arg4);

        RET ret = (obj_ptr->*(registed_data.dest_func))(arg1, arg2, arg3, arg4);
        lua_traits_t<RET>::push_stack(ls_, ret);
        return 1;
    }
};

template <typename CLASS_TYPE, typename FUNC_CLASS_TYPE, typename ARG1, typename ARG2, typename ARG3, typename ARG4,
          typename ARG5, typename RET>
struct class_register_impl_t<CLASS_TYPE, RET (FUNC_CLASS_TYPE::*)(ARG1, ARG2, ARG3, ARG4, ARG5) const>
{
    typedef RET (FUNC_CLASS_TYPE::*dest_func_t)(ARG1, ARG2, ARG3, ARG4, ARG5) const;
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int closure_function(lua_State* ls_)
    {
        //! 1. check user data, if arg type check failed, reutrn arg error info to lua.
        //! 2. get the class dest, func, exe it.

        void* dest_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_register_data_t& registed_data = *((dest_register_data_t*)dest_data);

        CLASS_TYPE* obj_ptr = check_userdata_type<CLASS_TYPE>(ls_);

        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::value();
        typename arg_traits_t<ARG2>::arg_type_t arg2 = init_value_traits_t<ARG2>::value();
        typename arg_traits_t<ARG3>::arg_type_t arg3 = init_value_traits_t<ARG3>::value();
        typename arg_traits_t<ARG4>::arg_type_t arg4 = init_value_traits_t<ARG4>::value();
        typename arg_traits_t<ARG5>::arg_type_t arg5 = init_value_traits_t<ARG5>::value();

        lua_traits_t<ARG1>::check_lua_param(ls_, ARG_INDEX(2), arg1);
        lua_traits_t<ARG2>::check_lua_param(ls_, ARG_INDEX(3), arg2);
        lua_traits_t<ARG3>::check_lua_param(ls_, ARG_INDEX(4), arg3);
        lua_traits_t<ARG4>::check_lua_param(ls_, ARG_INDEX(5), arg4);
        lua_traits_t<ARG5>::check_lua_param(ls_, ARG_INDEX(6), arg5);

        RET ret = (obj_ptr->*(registed_data.dest_func))(arg1, arg2, arg3, arg4, arg5);
        lua_traits_t<RET>::push_stack(ls_, ret);
        return 1;
    }
};


template <typename CLASS_TYPE, typename FUNC_CLASS_TYPE, typename ARG1, typename ARG2, typename ARG3, typename ARG4,
          typename ARG5, typename ARG6, typename RET>
struct class_register_impl_t<CLASS_TYPE, RET (FUNC_CLASS_TYPE::*)(ARG1, ARG2, ARG3, ARG4, ARG5,
                                                              ARG6) const>
{
    typedef RET (FUNC_CLASS_TYPE::*dest_func_t)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6) const;
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int closure_function(lua_State* ls_)
    {
        //! 1. check user data, if arg type check failed, reutrn arg error info to lua.
        //! 2. get the class dest, func, exe it.

        void* dest_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_register_data_t& registed_data = *((dest_register_data_t*)dest_data);

        CLASS_TYPE* obj_ptr = check_userdata_type<CLASS_TYPE>(ls_);

        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::value();
        typename arg_traits_t<ARG2>::arg_type_t arg2 = init_value_traits_t<ARG2>::value();
        typename arg_traits_t<ARG3>::arg_type_t arg3 = init_value_traits_t<ARG3>::value();
        typename arg_traits_t<ARG4>::arg_type_t arg4 = init_value_traits_t<ARG4>::value();
        typename arg_traits_t<ARG5>::arg_type_t arg5 = init_value_traits_t<ARG5>::value();
        typename arg_traits_t<ARG6>::arg_type_t arg6 = init_value_traits_t<ARG6>::value();

        lua_traits_t<ARG1>::check_lua_param(ls_, ARG_INDEX(2), arg1);
        lua_traits_t<ARG2>::check_lua_param(ls_, ARG_INDEX(3), arg2);
        lua_traits_t<ARG3>::check_lua_param(ls_, ARG_INDEX(4), arg3);
        lua_traits_t<ARG4>::check_lua_param(ls_, ARG_INDEX(5), arg4);
        lua_traits_t<ARG5>::check_lua_param(ls_, ARG_INDEX(6), arg5);
        lua_traits_t<ARG6>::check_lua_param(ls_, ARG_INDEX(7), arg6);

        RET ret = (obj_ptr->*(registed_data.dest_func))(arg1, arg2, arg3, arg4, arg5, arg6);
        lua_traits_t<RET>::push_stack(ls_, ret);
        return 1;
    }
};

template <typename CLASS_TYPE, typename FUNC_CLASS_TYPE, typename ARG1, typename ARG2, typename ARG3, typename ARG4,
          typename ARG5, typename ARG6, typename ARG7, typename RET>
struct class_register_impl_t<CLASS_TYPE, RET (FUNC_CLASS_TYPE::*)(ARG1, ARG2, ARG3, ARG4, ARG5,
                                                              ARG6, ARG7) const>
{
    typedef RET (FUNC_CLASS_TYPE::*dest_func_t)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7) const;
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int closure_function(lua_State* ls_)
    {
        //! 1. check user data, if arg type check failed, reutrn arg error info to lua.
        //! 2. get the class dest, func, exe it.

        void* dest_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_register_data_t& registed_data = *((dest_register_data_t*)dest_data);

        CLASS_TYPE* obj_ptr = check_userdata_type<CLASS_TYPE>(ls_);

        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::value();
        typename arg_traits_t<ARG2>::arg_type_t arg2 = init_value_traits_t<ARG2>::value();
        typename arg_traits_t<ARG3>::arg_type_t arg3 = init_value_traits_t<ARG3>::value();
        typename arg_traits_t<ARG4>::arg_type_t arg4 = init_value_traits_t<ARG4>::value();
        typename arg_traits_t<ARG5>::arg_type_t arg5 = init_value_traits_t<ARG5>::value();
        typename arg_traits_t<ARG6>::arg_type_t arg6 = init_value_traits_t<ARG6>::value();
        typename arg_traits_t<ARG7>::arg_type_t arg7 = init_value_traits_t<ARG7>::value();

        lua_traits_t<ARG1>::check_lua_param(ls_, ARG_INDEX(2), arg1);
        lua_traits_t<ARG2>::check_lua_param(ls_, ARG_INDEX(3), arg2);
        lua_traits_t<ARG3>::check_lua_param(ls_, ARG_INDEX(4), arg3);
        lua_traits_t<ARG4>::check_lua_param(ls_, ARG_INDEX(5), arg4);
        lua_traits_t<ARG5>::check_lua_param(ls_, ARG_INDEX(6), arg5);
        lua_traits_t<ARG6>::check_lua_param(ls_, ARG_INDEX(7), arg6);
        lua_traits_t<ARG7>::check_lua_param(ls_, ARG_INDEX(8), arg7);

        RET ret = (obj_ptr->*(registed_data.dest_func))(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
        lua_traits_t<RET>::push_stack(ls_, ret);
        return 1;
    }
};

template <typename CLASS_TYPE, typename FUNC_CLASS_TYPE, typename ARG1, typename ARG2, typename ARG3, typename ARG4,
          typename ARG5, typename ARG6, typename ARG7, typename ARG8, typename RET>
struct class_register_impl_t<CLASS_TYPE, RET (FUNC_CLASS_TYPE::*)(ARG1, ARG2, ARG3, ARG4, ARG5,
                                                                   ARG6, ARG7, ARG8) const>
{
    typedef RET (FUNC_CLASS_TYPE::*dest_func_t)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8) const;
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int closure_function(lua_State* ls_)
    {
        //! 1. check user data, if arg type check failed, reutrn arg error info to lua.
        //! 2. get the class dest, func, exe it.

        void* dest_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_register_data_t& registed_data = *((dest_register_data_t*)dest_data);

        CLASS_TYPE* obj_ptr = check_userdata_type<CLASS_TYPE>(ls_);

        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::value();
        typename arg_traits_t<ARG2>::arg_type_t arg2 = init_value_traits_t<ARG2>::value();
        typename arg_traits_t<ARG3>::arg_type_t arg3 = init_value_traits_t<ARG3>::value();
        typename arg_traits_t<ARG4>::arg_type_t arg4 = init_value_traits_t<ARG4>::value();
        typename arg_traits_t<ARG5>::arg_type_t arg5 = init_value_traits_t<ARG5>::value();
        typename arg_traits_t<ARG6>::arg_type_t arg6 = init_value_traits_t<ARG6>::value();
        typename arg_traits_t<ARG7>::arg_type_t arg7 = init_value_traits_t<ARG7>::value();
        typename arg_traits_t<ARG8>::arg_type_t arg8 = init_value_traits_t<ARG8>::value();

        lua_traits_t<ARG1>::check_lua_param(ls_, ARG_INDEX(2), arg1);
        lua_traits_t<ARG2>::check_lua_param(ls_, ARG_INDEX(3), arg2);
        lua_traits_t<ARG3>::check_lua_param(ls_, ARG_INDEX(4), arg3);
        lua_traits_t<ARG4>::check_lua_param(ls_, ARG_INDEX(5), arg4);
        lua_traits_t<ARG5>::check_lua_param(ls_, ARG_INDEX(6), arg5);
        lua_traits_t<ARG6>::check_lua_param(ls_, ARG_INDEX(7), arg6);
        lua_traits_t<ARG7>::check_lua_param(ls_, ARG_INDEX(8), arg7);
        lua_traits_t<ARG8>::check_lua_param(ls_, ARG_INDEX(9), arg8);

        RET ret = (obj_ptr->*(registed_data.dest_func))(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
        lua_traits_t<RET>::push_stack(ls_, ret);
        return 1;
    }
};


template <typename CLASS_TYPE, typename FUNC_CLASS_TYPE, typename ARG1, typename ARG2, typename ARG3, typename ARG4,
          typename ARG5, typename ARG6, typename ARG7, typename ARG8, typename ARG9, typename RET>
struct class_register_impl_t<CLASS_TYPE, RET (FUNC_CLASS_TYPE::*)(ARG1, ARG2, ARG3, ARG4, ARG5,
                                                              ARG6, ARG7, ARG8, ARG9) const>
{
    typedef RET (FUNC_CLASS_TYPE::*dest_func_t)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8, ARG9) const;
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int closure_function(lua_State* ls_)
    {
        //! 1. check user data, if arg type check failed, reutrn arg error info to lua.
        //! 2. get the class dest, func, exe it.

        void* dest_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_register_data_t& registed_data = *((dest_register_data_t*)dest_data);

        CLASS_TYPE* obj_ptr = check_userdata_type<CLASS_TYPE>(ls_);

        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::value();
        typename arg_traits_t<ARG2>::arg_type_t arg2 = init_value_traits_t<ARG2>::value();
        typename arg_traits_t<ARG3>::arg_type_t arg3 = init_value_traits_t<ARG3>::value();
        typename arg_traits_t<ARG4>::arg_type_t arg4 = init_value_traits_t<ARG4>::value();
        typename arg_traits_t<ARG5>::arg_type_t arg5 = init_value_traits_t<ARG5>::value();
        typename arg_traits_t<ARG6>::arg_type_t arg6 = init_value_traits_t<ARG6>::value();
        typename arg_traits_t<ARG7>::arg_type_t arg7 = init_value_traits_t<ARG7>::value();
        typename arg_traits_t<ARG8>::arg_type_t arg8 = init_value_traits_t<ARG8>::value();
        typename arg_traits_t<ARG9>::arg_type_t arg9 = init_value_traits_t<ARG9>::value();

        lua_traits_t<ARG1>::check_lua_param(ls_, ARG_INDEX(2), arg1);
        lua_traits_t<ARG2>::check_lua_param(ls_, ARG_INDEX(3), arg2);
        lua_traits_t<ARG3>::check_lua_param(ls_, ARG_INDEX(4), arg3);
        lua_traits_t<ARG4>::check_lua_param(ls_, ARG_INDEX(5), arg4);
        lua_traits_t<ARG5>::check_lua_param(ls_, ARG_INDEX(6), arg5);
        lua_traits_t<ARG6>::check_lua_param(ls_, ARG_INDEX(7), arg6);
        lua_traits_t<ARG7>::check_lua_param(ls_, ARG_INDEX(8), arg7);
        lua_traits_t<ARG8>::check_lua_param(ls_, ARG_INDEX(9), arg8);
        lua_traits_t<ARG9>::check_lua_param(ls_, ARG_INDEX(10), arg9);

        RET ret = (obj_ptr->*(registed_data.dest_func))(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
        lua_traits_t<RET>::push_stack(ls_, ret);
        return 1;
    }
};

//! ------------------------------------------------------------------- impl for property

template <typename CLASS_TYPE, typename PROPERTY_TYPE, typename RET>
struct class_register_impl_t<pair<CLASS_TYPE, lua_class_property_t>, RET (PROPERTY_TYPE::*)>
{

    typedef RET property_t;
    typedef RET PROPERTY_TYPE::* property_ptr_t;

    static  int newindex_function(lua_State* ls_)
    {
        //! 1. check user data, if arg type check failed, reutrn arg error info to lua.

        //! newindex function impl.
        string key;
        property_t  value   = init_value_traits_t<property_t>::value();

        CLASS_TYPE* obj_ptr = check_userdata_type<CLASS_TYPE>(ls_);
        lua_traits_t<string>::check_lua_param(ls_, ARG_INDEX(2), key);
        lua_traits_t<property_t>::check_lua_param(ls_, ARG_INDEX(3), value);

        property_ptr_t ptr = NULL;
        lua_type_registry_t<CLASS_TYPE>::get_field(key, ptr);

        if (ptr)
        {
            (obj_ptr->*ptr) = value;
        }
        else
        {
            cout <<"none this field<" << key << ">\n";
        }
        return 0;
    }

    static  int index_function(lua_State* ls_)
    {
        //! 1. check user data, if arg type check failed, reutrn arg error info to lua.

        //! newindex function impl.
        string key;
        property_t  value   = init_value_traits_t<property_t>::value();

        CLASS_TYPE* obj_ptr = check_userdata_type<CLASS_TYPE>(ls_);
        lua_traits_t<string>::check_lua_param(ls_, ARG_INDEX(2), key);
        lua_traits_t<property_t>::check_lua_param(ls_, ARG_INDEX(3), value);

        property_ptr_t ptr = NULL;
        lua_type_registry_t<CLASS_TYPE>::get_field(key, ptr);

        if (ptr)
        {
            (obj_ptr->*ptr) = value;
        }
        else
        {
            cout <<"none this field<" << key << ">\n";
        }
        return 0;
    }

    static int index_impl(lua_State* ls_, void* field_info_, const char* key_)
    {
        typedef property_registry_t<property_ptr_t> property_registry_t;
        CLASS_TYPE* obj_ptr = check_userdata_type<CLASS_TYPE>(ls_);

        property_registry_t* reg = (property_registry_t*)field_info_;
        property_ptr_t ptr = reg->field_ptr;

        if (ptr)
        {
            lua_traits_t<property_t>::push_stack(ls_, (obj_ptr->*ptr));
            return 1;
        }
        else
        {
            cout <<"none this field<" << key_ << ">\n";
            return 0;
        }

        return 0;
    }

    static int newindex_impl(lua_State* ls_, void* field_info_, const char* key_, int value_index_)
    {
        typedef property_registry_t<property_ptr_t> property_registry_t;
        CLASS_TYPE* obj_ptr = check_userdata_type<CLASS_TYPE>(ls_);

        property_registry_t* reg = (property_registry_t*)field_info_;
        property_ptr_t ptr = reg->field_ptr;

        if (ptr)
        {
            property_t  value   = init_value_traits_t<property_t>::value();
            lua_traits_t<property_t>::check_lua_param(ls_, value_index_, value);
            (obj_ptr->*ptr) = value;
            return 0;
        }
        else
        {
            cout <<"none this field<" << key_ << ">\n";
            return 0;
        }

        return 0;
    }

};

#endif
