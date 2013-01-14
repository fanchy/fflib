#ifndef _FF_LUA_H_
#define _FF_LUA_H_

#include <stdint.h>
#include <stdlib.h>
#include <lua.hpp>

#include <string>
using namespace std;

#include "lua_type.h"
#include "lua_macro.h"
#include "detail/lua_traits.h"

#include "detail/function_dispather.h"
#include "lua_object.h"

#include "detail/class_register.h"

class fflua_t
{
    enum stack_check_num_e
    {
        STACK_CHECK_NUM = 20
    };
public:
    fflua_t();
    virtual ~fflua_t();
    void stack_dump() const;

    lua_State* get_lua_state();

    int  add_package_path(const string& str_);
    int  load_file(const string& file_name_) throw (lua_exception_t);
    template<typename T>
    void open_lib(T arg_);
    void run_string(const string& str_) throw (lua_exception_t);
    void run_string(const char* str_) throw (lua_exception_t);

    template<typename T>
    int  get_global_variable(const string& field_name_, T& ret_);
    template<typename T>
    int  get_global_variable(const char* field_name_, T& ret_);

    template<typename T>
    int  set_global_variable(const string& field_name_, const T& value_);
    template<typename T>
    int  set_global_variable(const char* field_name_, const T& value_);

    void  register_raw_function(const char* func_name_, lua_function_t func_);
    template<typename FUNC>
    void  register_static_function(const char* mod_name_, const char* func_name_, FUNC func_);
    template<typename FUNC>
    void  register_global_function(const char* func_name_, FUNC func_);

    template<typename CLASS_TYPE, typename FUNC_TYPE>
    void  register_class_base(const string& class_name_);
    template<typename CLASS_TYPE, typename FUNC_TYPE>
    void  register_virtual_class_base(const string& class_name_);
    template<typename CLASS_TYPE, typename FUNC_TYPE>
    void  register_class_method(const string& class_name, const string& func_name, FUNC_TYPE func_);
    template<typename CLASS_TYPE, typename PROPERTY_TYPE>
    void  register_class_property(const string& property_name_, PROPERTY_TYPE p_);

    template<typename T>
    void  multi_register(T a);

    void call(const char* func_name_) throw (lua_exception_t);

    template<typename RET>
    RET call(const char* func_name_) throw (lua_exception_t);

    template<typename RET, typename ARG1>
    RET call(const char* func_name_, ARG1 arg1_) throw (lua_exception_t);

    template<typename RET, typename ARG1, typename ARG2>
    RET call(const char* func_name_, ARG1 arg1_, ARG2 arg2_) throw (lua_exception_t);

    template<typename RET, typename ARG1, typename ARG2, typename ARG3>
    RET call(const char* func_name_, ARG1 arg1_, ARG2 arg2_,
             ARG3 arg3_) throw (lua_exception_t);

    template<typename RET, typename ARG1, typename ARG2, typename ARG3, typename ARG4>
    RET call(const char* func_name_, ARG1 arg1_, ARG2 arg2_, ARG3 arg3_,
             ARG4 arg4_) throw (lua_exception_t);

    template<typename RET, typename ARG1, typename ARG2, typename ARG3, typename ARG4,
             typename ARG5>
    RET call(const char* func_name_, ARG1 arg1_, ARG2 arg2_, ARG3 arg3_,
             ARG4 arg4_, ARG5 arg5_) throw (lua_exception_t);

    template<typename RET, typename ARG1, typename ARG2, typename ARG3, typename ARG4,
             typename ARG5, typename ARG6>
    RET call(const char* func_name_, ARG1 arg1_, ARG2 arg2_, ARG3 arg3_,
             ARG4 arg4_, ARG5 arg5_, ARG6 arg6_) throw (lua_exception_t);

    template<typename RET, typename ARG1, typename ARG2, typename ARG3, typename ARG4,
             typename ARG5, typename ARG6, typename ARG7>
    RET call(const char* func_name_, ARG1 arg1_, ARG2 arg2_, ARG3 arg3_,
             ARG4 arg4_, ARG5 arg5_, ARG6 arg6_,
             ARG7 arg7_) throw (lua_exception_t);

    template<typename RET, typename ARG1, typename ARG2, typename ARG3, typename ARG4,
             typename ARG5, typename ARG6, typename ARG7, typename ARG8>
    RET call(const char* func_name_, ARG1 arg1_, ARG2 arg2_, ARG3 arg3_,
             ARG4 arg4_, ARG5 arg5_, ARG6 arg6_, ARG7 arg7_,
             ARG8 arg8_) throw (lua_exception_t);

    template<typename RET, typename ARG1, typename ARG2, typename ARG3, typename ARG4,
             typename ARG5, typename ARG6, typename ARG7, typename ARG8, typename ARG9>
    RET call(const char* func_name_, ARG1 arg1_, ARG2 arg2_, ARG3 arg3_,
             ARG4 arg4_, ARG5 arg5_, ARG6 arg6_, ARG7 arg7_,
             ARG8 arg8_, ARG9 arg9_) throw (lua_exception_t);

private:
    lua_State*  m_ls;
};

template<typename T>
void fflua_t::open_lib(T arg_)
{
    arg_(m_ls);
}

template<typename T>
int  fflua_t::get_global_variable(const string& field_name_, T& ret_)
{
    return get_global_variable<T>(field_name_.c_str(), ret_);
}

template<typename T>
int  fflua_t::get_global_variable(const char* field_name_, T& ret_)
{
     int ret = 0;

     lua_getglobal(m_ls, field_name_);
     ret = lua_traits_t<T>::check_param(m_ls, -1, ret_);

     lua_pop(m_ls, 1);
     return ret;
}

template<typename T>
int  fflua_t::set_global_variable(const string& field_name_, const T& value_)
{
    return set_global_variable<T>(field_name_.c_str(), value_);
}

template<typename T>
int  fflua_t::set_global_variable(const char* field_name_, const T& value_)
{
    lua_traits_t<T>::push_stack(m_ls, value_);
    lua_setglobal(m_ls, field_name_);
    return 0;
}

template<typename FUNC>
void  fflua_t::register_static_function(const char* mod_name_, const char* func_name_, FUNC func_)
{
    char buff[1024];
    const char* tmp_func_name = "__tmp_cpp_register_func__";

    //! if mod == nil then mod = {} end
    //! tmp_func = func
    //! mod.func_name = tmp_func
    //! tmp_func = nil

    snprintf(buff, sizeof(buff),
             "if %s == nil then %s = {} end %s.%s = %s %s = nil",
             mod_name_, mod_name_, mod_name_, func_name_, tmp_func_name, tmp_func_name);

    lua_checkstack(m_ls, STACK_CHECK_NUM);
    function_dispather_t::dispather_static_function(m_ls, func_);
    lua_setglobal(m_ls, tmp_func_name);

    run_string(buff);
}

template<typename FUNC>
void  fflua_t::register_global_function(const char* func_name_, FUNC func_)
{
    lua_checkstack(m_ls, STACK_CHECK_NUM);
    function_dispather_t::dispather_global_function(m_ls, func_);
    lua_setglobal(m_ls, func_name_);
}

template<typename CLASS_TYPE, typename FUNC_TYPE>
void  fflua_t::register_class_base(const string& class_name_)
{
    lua_checkstack(m_ls, STACK_CHECK_NUM);
    class_register_t::register_class_base<CLASS_TYPE, FUNC_TYPE>(m_ls, class_name_);
}

template<typename CLASS_TYPE, typename FUNC_TYPE>
void  fflua_t::register_virtual_class_base(const string& class_name_)
{
    lua_checkstack(m_ls, STACK_CHECK_NUM);
    class_register_t::register_virtual_class_base<CLASS_TYPE, FUNC_TYPE>(m_ls, class_name_);
}

template<typename CLASS_TYPE, typename FUNC_TYPE>
void  fflua_t::register_class_method(const string& class_name_, const string& func_name_, FUNC_TYPE func_)
{
    lua_checkstack(m_ls, STACK_CHECK_NUM);
    class_register_t::register_class_method<CLASS_TYPE>(m_ls, class_name_, func_name_, func_);
}

template<typename CLASS_TYPE, typename PROPERTY_TYPE>
void  fflua_t::register_class_property(const string& property_name_, PROPERTY_TYPE p_)
{
    lua_checkstack(m_ls, STACK_CHECK_NUM);
    class_register_t::register_class_property<CLASS_TYPE>(m_ls, property_name_, p_);
}

template<typename T>
void  fflua_t::multi_register(T a)
{
    a(*this);
}

//! impl for common RET
template<typename RET>
RET fflua_t::call(const char* func_name_) throw (lua_exception_t)
{
    RET ret = init_value_traits_t<RET>::value();

    lua_getglobal(m_ls, lua_string_tool_t::c_str(func_name_));

    if (lua_pcall(m_ls, 0, 1, 0) != 0)
    {
        string err = lua_helper::dump_error(m_ls, "lua_pcall failed func_name[%s]", func_name_);
        lua_pop(m_ls, 1);
        throw lua_exception_t(err);
    }

    if (lua_traits_t<RET>::check_param(m_ls, -1, ret))
    {
        lua_pop(m_ls, 1);
        char buff[512];
        snprintf(buff, sizeof(buff), "callfunc [arg0] check_param failed  func_name[%s]", func_name_);
        throw lua_exception_t(buff);
    }

    lua_pop(m_ls, 1);

    return ret;
}


template<typename RET, typename ARG1>
RET fflua_t::call(const char* func_name_, ARG1 arg1_) throw (lua_exception_t)
{
    RET ret = init_value_traits_t<RET>::value();

    lua_getglobal(m_ls, lua_string_tool_t::c_str(func_name_));

    lua_traits_t<ARG1>::push_stack(m_ls, arg1_);

    if (lua_pcall(m_ls, 1, 1, 0) != 0)
    {
        string err = lua_helper::dump_error(m_ls, "lua_pcall failed func_name[%s]", func_name_);
        lua_pop(m_ls, 1);
        throw lua_exception_t(err);
    }

    if (lua_traits_t<RET>::check_param(m_ls, -1, ret))
    {
        lua_pop(m_ls, 1);
        char buff[512];
        snprintf(buff, sizeof(buff), "callfunc [arg1] check_param failed  func_name[%s]", func_name_);
        throw lua_exception_t(buff);
    }

    lua_pop(m_ls, 1);

    return ret;
}


template<typename RET, typename ARG1, typename ARG2>
RET fflua_t::call(const char* func_name_, ARG1 arg1_, ARG2 arg2_)
                                 throw (lua_exception_t)
{
    RET ret = init_value_traits_t<RET>::value();

    lua_getglobal(m_ls, lua_string_tool_t::c_str(func_name_));

    lua_traits_t<ARG1>::push_stack(m_ls, arg1_);
    lua_traits_t<ARG2>::push_stack(m_ls, arg2_);

    if (lua_pcall(m_ls, 2, 1, 0) != 0)
    {
        string err = lua_helper::dump_error(m_ls, "lua_pcall failed func_name[%s]", func_name_);
        lua_pop(m_ls, 1);
        throw lua_exception_t(err);
    }

    if (lua_traits_t<RET>::check_param(m_ls, -1, ret))
    {
        lua_pop(m_ls, 1);
        char buff[512];
        snprintf(buff, sizeof(buff), "callfunc [arg2] check_param failed  func_name[%s]", func_name_);
        throw lua_exception_t(buff);
    }

    lua_pop(m_ls, 1);

    return ret;
}

template<typename RET, typename ARG1, typename ARG2, typename ARG3>
RET fflua_t::call(const char* func_name_, ARG1 arg1_, ARG2 arg2_,
                                 ARG3 arg3_) throw (lua_exception_t)
{
    RET ret = init_value_traits_t<RET>::value();

    lua_getglobal(m_ls, lua_string_tool_t::c_str(func_name_));

    lua_traits_t<ARG1>::push_stack(m_ls, arg1_);
    lua_traits_t<ARG2>::push_stack(m_ls, arg2_);
    lua_traits_t<ARG3>::push_stack(m_ls, arg3_);

    if (lua_pcall(m_ls, 3, 1, 0) != 0)
    {
        string err = lua_helper::dump_error(m_ls, "lua_pcall failed func_name[%s]", func_name_);
        lua_pop(m_ls, 1);
        throw lua_exception_t(err);
    }

    if (lua_traits_t<RET>::check_param(m_ls, -1, ret))
    {
        lua_pop(m_ls, 1);
        char buff[512];
        snprintf(buff, sizeof(buff), "callfunc [arg3] check_param failed  func_name[%s]", func_name_);
        throw lua_exception_t(buff);
    }

    lua_pop(m_ls, 1);

    return ret;
}

template<typename RET, typename ARG1, typename ARG2, typename ARG3, typename ARG4>
RET fflua_t::call(const char* func_name_, ARG1 arg1_, ARG2 arg2_, ARG3 arg3_,
                                 ARG4 arg4_) throw (lua_exception_t)
{
    RET ret = init_value_traits_t<RET>::value();

    lua_getglobal(m_ls, lua_string_tool_t::c_str(func_name_));

    lua_traits_t<ARG1>::push_stack(m_ls, arg1_);
    lua_traits_t<ARG2>::push_stack(m_ls, arg2_);
    lua_traits_t<ARG3>::push_stack(m_ls, arg3_);
    lua_traits_t<ARG4>::push_stack(m_ls, arg4_);

    if (lua_pcall(m_ls, 4, 1, 0) != 0)
    {
        string err = lua_helper::dump_error(m_ls, "lua_pcall failed func_name[%s]", func_name_);
        lua_pop(m_ls, 1);
        throw lua_exception_t(err);
    }

    if (lua_traits_t<RET>::check_param(m_ls, -1, ret))
    {
        lua_pop(m_ls, 1);
        char buff[512];
        snprintf(buff, sizeof(buff), "callfunc [arg4] check_param failed  func_name[%s]", func_name_);
        throw lua_exception_t(buff);
    }

    lua_pop(m_ls, 1);

    return ret;
}

template<typename RET, typename ARG1, typename ARG2, typename ARG3, typename ARG4, typename ARG5>
RET fflua_t::call(const char* func_name_, ARG1 arg1_, ARG2 arg2_, ARG3 arg3_,
                                 ARG4 arg4_, ARG5 arg5_) throw (lua_exception_t)
{
    RET ret = init_value_traits_t<RET>::value();

    lua_getglobal(m_ls, lua_string_tool_t::c_str(func_name_));

    lua_traits_t<ARG1>::push_stack(m_ls, arg1_);
    lua_traits_t<ARG2>::push_stack(m_ls, arg2_);
    lua_traits_t<ARG3>::push_stack(m_ls, arg3_);
    lua_traits_t<ARG4>::push_stack(m_ls, arg4_);
    lua_traits_t<ARG5>::push_stack(m_ls, arg5_);

    if (lua_pcall(m_ls, 5, 1, 0) != 0)
    {
        string err = lua_helper::dump_error(m_ls, "lua_pcall failed func_name[%s]", func_name_);
        lua_pop(m_ls, 1);
        throw lua_exception_t(err);
    }

    if (lua_traits_t<RET>::check_param(m_ls, -1, ret))
    {
        lua_pop(m_ls, 1);
        char buff[512];
        snprintf(buff, sizeof(buff), "callfunc [arg5] check_param failed  func_name[%s]", func_name_);
        throw lua_exception_t(buff);
    }

    lua_pop(m_ls, 1);

    return ret;
}


template<typename RET, typename ARG1, typename ARG2, typename ARG3, typename ARG4, typename ARG5, typename ARG6>
RET fflua_t::call(const char* func_name_, ARG1 arg1_, ARG2 arg2_, ARG3 arg3_,
                                 ARG4 arg4_, ARG5 arg5_, ARG6 arg6_) throw (lua_exception_t)
{
    RET ret = init_value_traits_t<RET>::value();

    lua_getglobal(m_ls, lua_string_tool_t::c_str(func_name_));

    lua_traits_t<ARG1>::push_stack(m_ls, arg1_);
    lua_traits_t<ARG2>::push_stack(m_ls, arg2_);
    lua_traits_t<ARG3>::push_stack(m_ls, arg3_);
    lua_traits_t<ARG4>::push_stack(m_ls, arg4_);
    lua_traits_t<ARG5>::push_stack(m_ls, arg5_);
    lua_traits_t<ARG6>::push_stack(m_ls, arg6_);

    if (lua_pcall(m_ls, 6, 1, 0) != 0)
    {
        string err = lua_helper::dump_error(m_ls, "lua_pcall failed func_name[%s]", func_name_);
        lua_pop(m_ls, 1);
        throw lua_exception_t(err);
    }

    if (lua_traits_t<RET>::check_param(m_ls, -1, ret))
    {
        lua_pop(m_ls, 1);
        char buff[512];
        snprintf(buff, sizeof(buff), "callfunc [arg6] check_param failed  func_name[%s]", func_name_);
        throw lua_exception_t(buff);
    }

    lua_pop(m_ls, 1);

    return ret;
}


template<typename RET, typename ARG1, typename ARG2, typename ARG3, typename ARG4,
                typename ARG5, typename ARG6, typename ARG7>
RET fflua_t::call(const char* func_name_, ARG1 arg1_, ARG2 arg2_, ARG3 arg3_,
                                 ARG4 arg4_, ARG5 arg5_, ARG6 arg6_,
                                 ARG7 arg7_) throw (lua_exception_t)
{
    RET ret = init_value_traits_t<RET>::value();

    lua_getglobal(m_ls, lua_string_tool_t::c_str(func_name_));

    lua_traits_t<ARG1>::push_stack(m_ls, arg1_);
    lua_traits_t<ARG2>::push_stack(m_ls, arg2_);
    lua_traits_t<ARG3>::push_stack(m_ls, arg3_);
    lua_traits_t<ARG4>::push_stack(m_ls, arg4_);
    lua_traits_t<ARG5>::push_stack(m_ls, arg5_);
    lua_traits_t<ARG6>::push_stack(m_ls, arg6_);
    lua_traits_t<ARG7>::push_stack(m_ls, arg7_);

    if (lua_pcall(m_ls, 7, 1, 0) != 0)
    {
        string err = lua_helper::dump_error(m_ls, "lua_pcall failed func_name[%s]", func_name_);
        lua_pop(m_ls, 1);
        throw lua_exception_t(err);
    }

    if (lua_traits_t<RET>::check_param(m_ls, -1, ret))
    {
        lua_pop(m_ls, 1);
        char buff[512];
        snprintf(buff, sizeof(buff), "callfunc [arg7] check_param failed  func_name[%s]", func_name_);
        throw lua_exception_t(buff);
    }

    lua_pop(m_ls, 1);

    return ret;
}


template<typename RET, typename ARG1, typename ARG2, typename ARG3, typename ARG4,
                typename ARG5, typename ARG6, typename ARG7, typename ARG8>
RET fflua_t::call(const char* func_name_, ARG1 arg1_, ARG2 arg2_, ARG3 arg3_,
                                 ARG4 arg4_, ARG5 arg5_, ARG6 arg6_, ARG7 arg7_,
                                 ARG8 arg8_) throw (lua_exception_t)
{
    RET ret = init_value_traits_t<RET>::value();

    lua_getglobal(m_ls, lua_string_tool_t::c_str(func_name_));

    lua_traits_t<ARG1>::push_stack(m_ls, arg1_);
    lua_traits_t<ARG2>::push_stack(m_ls, arg2_);
    lua_traits_t<ARG3>::push_stack(m_ls, arg3_);
    lua_traits_t<ARG4>::push_stack(m_ls, arg4_);
    lua_traits_t<ARG5>::push_stack(m_ls, arg5_);
    lua_traits_t<ARG6>::push_stack(m_ls, arg6_);
    lua_traits_t<ARG7>::push_stack(m_ls, arg7_);
    lua_traits_t<ARG8>::push_stack(m_ls, arg8_);

    if (lua_pcall(m_ls, 8, 1, 0) != 0)
    {
        string err = lua_helper::dump_error(m_ls, "lua_pcall failed func_name[%s]", func_name_);
        lua_pop(m_ls, 1);
        throw lua_exception_t(err);
    }

    if (lua_traits_t<RET>::check_param(m_ls, -1, ret))
    {
        lua_pop(m_ls, 1);
        char buff[512];
        snprintf(buff, sizeof(buff), "callfunc [arg8] check_param failed  func_name[%s]", func_name_);
        throw lua_exception_t(buff);
    }

    lua_pop(m_ls, 1);

    return ret;
}


template<typename RET, typename ARG1, typename ARG2, typename ARG3, typename ARG4,
                typename ARG5, typename ARG6, typename ARG7, typename ARG8, typename ARG9>
RET fflua_t::call(const char* func_name_, ARG1 arg1_, ARG2 arg2_, ARG3 arg3_,
                                 ARG4 arg4_, ARG5 arg5_, ARG6 arg6_, ARG7 arg7_,
                                 ARG8 arg8_, ARG9 arg9_) throw (lua_exception_t)
{
    RET ret = init_value_traits_t<RET>::value();

    lua_getglobal(m_ls, lua_string_tool_t::c_str(func_name_));

    lua_traits_t<ARG1>::push_stack(m_ls, arg1_);
    lua_traits_t<ARG2>::push_stack(m_ls, arg2_);
    lua_traits_t<ARG3>::push_stack(m_ls, arg3_);
    lua_traits_t<ARG4>::push_stack(m_ls, arg4_);
    lua_traits_t<ARG5>::push_stack(m_ls, arg5_);
    lua_traits_t<ARG6>::push_stack(m_ls, arg6_);
    lua_traits_t<ARG7>::push_stack(m_ls, arg7_);
    lua_traits_t<ARG8>::push_stack(m_ls, arg8_);
    lua_traits_t<ARG9>::push_stack(m_ls, arg9_);

    if (lua_pcall(m_ls, 9, 1, 0) != 0)
    {
        string err = lua_helper::dump_error(m_ls, "lua_pcall failed func_name[%s]", func_name_);
        lua_pop(m_ls, 1);
        throw lua_exception_t(err);
    }

    if (lua_traits_t<RET>::check_param(m_ls, -1, ret))
    {
        lua_pop(m_ls, 1);
        char buff[512];
        snprintf(buff, sizeof(buff), "callfunc [arg9] check_param failed  func_name[%s]", func_name_);
        throw lua_exception_t(buff);
    }

    lua_pop(m_ls, 1);

    return ret;
}

#endif
