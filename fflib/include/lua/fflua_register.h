#ifndef _FF_LUA_REGISTER_H_
#define _FF_LUA_REGISTER_H_

#include <lua.hpp>
#include <string>
#include <string.h>
#include <stdio.h>
using namespace std;

#include "lua/fflua_type.h"

namespace ff
{

#define ctor void
#define ARG_INDEX(x) (x)
#define LUA_ARG_INDEX(x) (x)
typedef int (*mt_index_func_t)(lua_State*, void*, const char*);
typedef int (*mt_newindex_func_t)(lua_State*, void*, const char*, int);

#define METATABLE_NAME(x) metatable_name((x)).c_str()
static string metatable_name(const string& name_)
{
	string ret = "fflua." + name_;
	return ret;
}


template<typename T>
struct property_registry_t
{
    property_registry_t():field_ptr(NULL)
    {
    }
    T field_ptr;
};

//! 记录类中字段的指针
struct lua_property_impl_t
{
    lua_property_impl_t():
        index_impl_func(NULL),
        newindex_impl_func(NULL),
        field_ptr(NULL)
    {}
    //! index impl function and newindex impl function
    mt_index_func_t    index_impl_func;
    mt_newindex_func_t newindex_impl_func;

    void* field_ptr;
};

template<typename FUNC_TYPE>
struct register_data_t
{
    register_data_t(FUNC_TYPE func_):
        dest_func(func_)
    {
    }

    FUNC_TYPE   dest_func;
};
template<typename PROPERTY_TYPE>
struct lua_property_udata_t : public lua_property_impl_t
{
    typedef property_registry_t<PROPERTY_TYPE> reg_property_t;
    reg_property_t                             reg_property;
};


//!  生成构造函数new,delete,index的实现类
template<typename CLASS_TYPE>
struct metatable_register_impl_t
{
	static int mt_index_function(lua_State* ls_)
	{
		string key;
		lua_traits_t<string>::check_lua_param(ls_, ARG_INDEX(2), key);

		luaL_getmetatable(ls_, lua_type_info_t<CLASS_TYPE>::get_name());
		int mt_index = lua_gettop(ls_);

		lua_getfield(ls_, -1, key.c_str());
		lua_remove(ls_, mt_index);

		if (lua_isuserdata(ls_, -1))//! 获取属性
		{
			lua_property_impl_t* p = (lua_property_impl_t*)lua_touserdata(ls_, -1);
			lua_pop(ls_, 1);
			return (*(p->index_impl_func))(ls_, p->field_ptr, key.c_str());
		}
		else
		{
			return 1;
		}
	}
	static int mt_newindex_function(lua_State* ls_)
	{
		string key;
		lua_traits_t<string>::check_lua_param(ls_, ARG_INDEX(2), key);

		luaL_getmetatable(ls_, lua_type_info_t<CLASS_TYPE>::get_name());
		int mt_index = lua_gettop(ls_);

		lua_getfield(ls_, -1, key.c_str());
		lua_remove(ls_, mt_index);

		if (lua_isuserdata(ls_, -1))
		{
			lua_property_impl_t* p = (lua_property_impl_t*)lua_touserdata(ls_, -1);
			lua_pop(ls_, 1);
			return (*(p->newindex_impl_func))(ls_, p->field_ptr, key.c_str(), ARG_INDEX(3));
		}
		else
		{
			return 1;
		}
	}

	static CLASS_TYPE** get_userdata_ref(lua_State* ls_)
	{
	    if (false == lua_type_info_t<CLASS_TYPE>::is_registed())
	    {
	        luaL_argerror(ls_, 1, "arg 1 can't convert to class*, because the class has not registed to Lua");
	    }

	    void* arg_data = luaL_checkudata(ls_, 1, lua_type_info_t<CLASS_TYPE>::get_name());

	    if (NULL == arg_data)
	    {
	        char buff[512];
	        snprintf(buff, sizeof(buff), "`%s` expect arg 1, but arg == null",
	        								lua_type_info_t<CLASS_TYPE>::get_name());
	        luaL_argerror(ls_, 1, buff);
	    }

	    CLASS_TYPE** ret_ptr = &(((class_userdata_t<CLASS_TYPE>*)arg_data)->obj_ptr);
	    return ret_ptr;
	}
	static CLASS_TYPE* check_userdata_type(lua_State* ls_)
	{
	    if (false == lua_type_info_t<CLASS_TYPE>::is_registed())
	    {
	        luaL_argerror(ls_, 1, "arg 1 can't convert to class*, because the class has not registed to Lua");
	    }

	    void* arg_data = luaL_checkudata(ls_, 1, lua_type_info_t<CLASS_TYPE>::get_name());

	    if (NULL == arg_data)
	    {
	        char buff[512];
	        snprintf(buff, sizeof(buff), "`%s` expect arg 1, but arg == null",
	        								lua_type_info_t<CLASS_TYPE>::get_name());
	        luaL_argerror(ls_, 1, buff);
	    }

	    CLASS_TYPE* ret_ptr = ((class_userdata_t<CLASS_TYPE>*)arg_data)->obj_ptr;
	    if (NULL == ret_ptr)
	    {
	        char buff[512];
	        snprintf(buff, sizeof(buff), "`%s` object ptr can't be null",
	        								lua_type_info_t<CLASS_TYPE>::get_name());
	        luaL_argerror(ls_, 1, buff);
	    }
	    return ret_ptr;
	}
	static int get_pointer(lua_State* ls_)
	{
		CLASS_TYPE** obj_ptr = get_userdata_ref(ls_);
		long  addr = long(*obj_ptr);
		lua_traits_t<long>::push_stack(ls_, addr);
		return 1;
	}
};


template <typename CLASS_TYPE, typename FUNC_TYPE>
struct new_traits_t;

template <typename CLASS_TYPE>
struct delete_traits_t
{
    typedef void(*dest_func_t)();
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int lua_function(lua_State* ls_)
    {
        CLASS_TYPE** obj_ptr = metatable_register_impl_t<CLASS_TYPE>::get_userdata_ref(ls_);

        delete *obj_ptr;
        *obj_ptr = NULL;
        return 0;
    }
};

template <typename FUNC_TYPE>
struct class_function_traits_t;
template <typename PROPERTY_TYPE, typename RET>
struct class_property_traits_t;
template <typename FUNC_TYPE>
struct function_traits_t;

//! CLASS_TYPE 为要注册的类型, CTOR_TYPE为构造函数类型
template<typename CLASS_TYPE, typename CTOR_TYPE = void()>
class fflua_register_t
{
public:
	fflua_register_t(lua_State* ls_, const string& class_name_);


	template<typename FUNC_TYPE>
	fflua_register_t& def(FUNC_TYPE func, const string& s_)
	{
		//TODO
	}
	template<typename FUNC_TYPE>
	fflua_register_t& def_class_func(FUNC_TYPE func_, const string& func_name_)
	{
		lua_function_t class_function = &class_function_traits_t<FUNC_TYPE>::lua_function;
		typedef typename class_function_traits_t<FUNC_TYPE>::dest_register_data_t register_data_t;
		void* user_data_ptr = lua_newuserdata(m_ls, sizeof(register_data_t));
		new(user_data_ptr) register_data_t(func_);
		lua_pushcclosure(m_ls, class_function, 1);

		luaL_getmetatable(m_ls, METATABLE_NAME(m_class_name));
		lua_pushstring(m_ls, func_name_.c_str());
		lua_pushvalue(m_ls, -3);
		lua_settable(m_ls, -3);

		lua_pop(m_ls, 2);
		return *this;
	}
	template<typename RET>
	fflua_register_t& def_class_property(RET CLASS_TYPE::* p_, const string& property_name_)
	{
		index_impl_func_t index_func       = &class_property_traits_t<CLASS_TYPE, RET>::process_index;
		newindex_impl_func_t newindex_func = &class_property_traits_t<CLASS_TYPE, RET>::process_newindex;

		typedef lua_property_udata_t<RET CLASS_TYPE::*> reg_property_udata_t;

		reg_property_udata_t* ptr_udata = (reg_property_udata_t*)lua_newuserdata(m_ls, sizeof(reg_property_udata_t));
		ptr_udata->reg_property.field_ptr = p_;

		int udata_index = lua_gettop(m_ls);
		ptr_udata->index_impl_func = index_func;
		ptr_udata->newindex_impl_func = newindex_func;
		ptr_udata->field_ptr = (void*)(&(ptr_udata->reg_property));

		luaL_getmetatable(m_ls, lua_type_info_t<CLASS_TYPE>::get_name());
		lua_pushstring(m_ls, property_name_.c_str());
		lua_pushvalue(m_ls, udata_index);
		lua_settable(m_ls, -3);

		lua_pop(m_ls, 1);
		lua_remove(m_ls, udata_index);
		return *this;
	}
	template<typename FUNC>
	fflua_register_t& def_func(FUNC func_, const string& func_name_)
	{
		lua_function_t lua_func = function_traits_t<FUNC>::lua_function;

		void* user_data_ptr = lua_newuserdata(m_ls, sizeof(func_));
		new(user_data_ptr) FUNC(func_);

		lua_pushcclosure(m_ls, lua_func, 1);
		lua_setglobal(m_ls, func_name_.c_str());
		return *this;
	}
private:
	lua_State* 	m_ls;
	string 		m_class_name;
};

template<typename CLASS_TYPE, typename CTOR_TYPE>
fflua_register_t<CLASS_TYPE, CTOR_TYPE>::fflua_register_t(lua_State* ls_, const string& class_name_):
	m_ls(ls_),
	m_class_name(class_name_)
{
	lua_type_info_t<CLASS_TYPE>::set_name(METATABLE_NAME(class_name_));

	luaL_newmetatable(ls_, METATABLE_NAME(class_name_));
	int  metatable_index = lua_gettop(ls_);

	lua_pushstring(ls_, "__index");
	lua_function_t index_function = &metatable_register_impl_t<CLASS_TYPE>::mt_index_function;
	lua_pushcclosure(ls_, index_function, 0);
	lua_settable(ls_, -3);

	lua_pushstring(ls_, "get_pointer");
	lua_function_t pointer_function = &metatable_register_impl_t<CLASS_TYPE>::get_pointer;
	lua_pushcclosure(ls_, pointer_function, 0);
	lua_settable(ls_, -3);

	lua_pushstring(ls_, "__newindex");
	lua_function_t newindex_function = &metatable_register_impl_t<CLASS_TYPE>::mt_newindex_function;
	lua_pushcclosure(ls_, newindex_function, 0);
	lua_settable(ls_, -3);

	lua_function_t function_for_new = &new_traits_t<CLASS_TYPE, CTOR_TYPE>::lua_function;
	lua_pushcclosure(ls_, function_for_new, 0);

	lua_newtable(ls_);
	lua_pushstring(ls_, "new");
	lua_pushvalue(ls_, -3);
	lua_settable(ls_, -3);

	lua_setglobal(ls_, class_name_.c_str());

	lua_pop(ls_, 1);

	lua_function_t function_for_delete = &delete_traits_t<CLASS_TYPE>::lua_function;
	lua_pushcclosure(ls_, function_for_delete, 0);

	lua_pushstring(ls_, "delete");
	lua_pushvalue(ls_, -2);
	lua_settable(ls_, metatable_index);

	lua_pop(ls_, 2);
}


template <typename CLASS_TYPE>
struct new_traits_t<CLASS_TYPE, void()>
{
    typedef void(*dest_func_t)();
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int lua_function(lua_State* ls_)
    {
        void* user_data_ptr = lua_newuserdata(ls_, sizeof(class_userdata_t<CLASS_TYPE>));
        luaL_getmetatable(ls_, lua_type_info_t<CLASS_TYPE>::get_name());
        lua_setmetatable(ls_, -2);

        new(user_data_ptr) class_userdata_t<CLASS_TYPE>(new CLASS_TYPE());
        return 1;
    }
};

template <typename CLASS_TYPE, typename ARG1>
struct new_traits_t<CLASS_TYPE, void(ARG1)>
{
    typedef void(*dest_func_t)();
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int lua_function(lua_State* ls_)
    {
        //! 1. get closuser userdata. metatable(or typename) store in userdata.
        //! 2. check arg valid.
        //! 3. construct class instance, set the metatable
        //! 4. return userdata to lua.

        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::value();
        lua_traits_t<ARG1>::check_lua_param(ls_, ARG_INDEX(2), arg1);

        void* user_data_ptr = lua_newuserdata(ls_, sizeof(class_userdata_t<CLASS_TYPE>));
        luaL_getmetatable(ls_, lua_type_info_t<CLASS_TYPE>::get_name());
        lua_setmetatable(ls_, -2);

        new(user_data_ptr) class_userdata_t<CLASS_TYPE>(new CLASS_TYPE(arg1));
        return 1;
    }
};


template <typename CLASS_TYPE, typename ARG1, typename ARG2>
struct new_traits_t<CLASS_TYPE, void(ARG1, ARG2)>
{
    static  int lua_function(lua_State* ls_)
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
        luaL_getmetatable(ls_, lua_type_info_t<CLASS_TYPE>::get_name());
        lua_setmetatable(ls_, -2);

        new(user_data_ptr) class_userdata_t<CLASS_TYPE>(new CLASS_TYPE(arg1, arg2));
        return 1;
    }
};

template <typename CLASS_TYPE, typename ARG1, typename ARG2, typename ARG3>
struct new_traits_t<CLASS_TYPE, void(ARG1, ARG2, ARG3)>
{
    static  int lua_function(lua_State* ls_)
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

        luaL_getmetatable(ls_, lua_type_info_t<CLASS_TYPE>::get_name());
        lua_setmetatable(ls_, -2);

        new(user_data_ptr) class_userdata_t<CLASS_TYPE>(new CLASS_TYPE(arg1, arg2, arg3));
        return 1;
    }
};

template <typename CLASS_TYPE, typename ARG1, typename ARG2, typename ARG3,
          typename ARG4>
struct new_traits_t<CLASS_TYPE, void(ARG1, ARG2, ARG3, ARG4)>
{
    static  int lua_function(lua_State* ls_)
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
        luaL_getmetatable(ls_, lua_type_info_t<CLASS_TYPE>::get_name());
        lua_setmetatable(ls_, -2);

        new(user_data_ptr) class_userdata_t<CLASS_TYPE>(new CLASS_TYPE(arg1, arg2, arg3, arg4));
        return 1;
    }
};

template <typename CLASS_TYPE, typename ARG1, typename ARG2, typename ARG3,
          typename ARG4, typename ARG5>
struct new_traits_t<CLASS_TYPE, void(ARG1, ARG2, ARG3, ARG4,
                                               ARG5)>
{
    static  int lua_function(lua_State* ls_)
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
        luaL_getmetatable(ls_, lua_type_info_t<CLASS_TYPE>::get_name());
        lua_setmetatable(ls_, -2);

        new(user_data_ptr) class_userdata_t<CLASS_TYPE>(new CLASS_TYPE(arg1, arg2, arg3, arg4, arg5));
        return 1;
    }
};

template <typename CLASS_TYPE, typename ARG1, typename ARG2, typename ARG3,
          typename ARG4, typename ARG5, typename ARG6>
struct new_traits_t<CLASS_TYPE, void(ARG1, ARG2, ARG3,
                                               ARG4, ARG5, ARG6)>
{
    static  int lua_function(lua_State* ls_)
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
        luaL_getmetatable(ls_, lua_type_info_t<CLASS_TYPE>::get_name());
        lua_setmetatable(ls_, -2);

        new(user_data_ptr) class_userdata_t<CLASS_TYPE>(new CLASS_TYPE(arg1, arg2, arg3, arg4, arg5, arg6));
        return 1;
    }
};

template <typename CLASS_TYPE, typename ARG1, typename ARG2, typename ARG3,
          typename ARG4, typename ARG5, typename ARG6, typename ARG7>
struct new_traits_t<CLASS_TYPE, void(ARG1, ARG2, ARG3,
                                               ARG4, ARG5, ARG6, ARG7)>
{
    static  int lua_function(lua_State* ls_)
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

        luaL_getmetatable(ls_, lua_type_info_t<CLASS_TYPE>::get_name());
        lua_setmetatable(ls_, -2);

        new(user_data_ptr) class_userdata_t<CLASS_TYPE>(new CLASS_TYPE(arg1, arg2, arg3, arg4, arg5,
                                                        arg6, arg7));
        return 1;
    }
};

template <typename CLASS_TYPE, typename ARG1, typename ARG2, typename ARG3,
          typename ARG4, typename ARG5, typename ARG6, typename ARG7, typename ARG8>
struct new_traits_t<CLASS_TYPE, void(ARG1, ARG2, ARG3,
                                               ARG4, ARG5, ARG6, ARG7, ARG8)>
{
    static  int lua_function(lua_State* ls_)
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
        luaL_getmetatable(ls_, lua_type_info_t<CLASS_TYPE>::get_name());
        lua_setmetatable(ls_, -2);

        new(user_data_ptr) class_userdata_t<CLASS_TYPE>(new CLASS_TYPE(arg1, arg2, arg3, arg4, arg5, arg6,
                                                        arg7, arg8));
        return 1;
    }
};

template <typename CLASS_TYPE, typename ARG1, typename ARG2, typename ARG3,
          typename ARG4, typename ARG5, typename ARG6, typename ARG7, typename ARG8,
          typename ARG9>
struct new_traits_t<CLASS_TYPE, void(ARG1, ARG2, ARG3,
                                               ARG4, ARG5, ARG6, ARG7, ARG8, ARG9)>
{
    static  int lua_function(lua_State* ls_)
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
        luaL_getmetatable(ls_, lua_type_info_t<CLASS_TYPE>::get_name());
        lua_setmetatable(ls_, -2);

        new(user_data_ptr) class_userdata_t<CLASS_TYPE>(new CLASS_TYPE(arg1, arg2, arg3, arg4, arg5, arg6,
                                                        arg7, arg8, arg9));

        return 1;
    }
};

template <typename FUNC_CLASS_TYPE>
struct class_function_traits_t<void (FUNC_CLASS_TYPE::*)()>
{
    typedef void (FUNC_CLASS_TYPE::*dest_func_t)();
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int lua_function(lua_State* ls_)
    {
    	void* dest_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_register_data_t& registed_data = *((dest_register_data_t*)dest_data);

        FUNC_CLASS_TYPE* obj_ptr = metatable_register_impl_t<FUNC_CLASS_TYPE>::check_userdata_type(ls_);
        (obj_ptr->*(registed_data.dest_func))();
        return 0;
    }
};

template <typename FUNC_CLASS_TYPE, typename ARG1>
struct class_function_traits_t<void (FUNC_CLASS_TYPE::*)(ARG1)>
{
    typedef void (FUNC_CLASS_TYPE::*dest_func_t)(ARG1);
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int lua_function(lua_State* ls_)
    {
        //! 1. check user data, if arg type check failed, reutrn arg error info to lua.
        //! 2. get the class dest, func, exe it.

        void* dest_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_register_data_t& registed_data = *((dest_register_data_t*)dest_data);

        FUNC_CLASS_TYPE* obj_ptr = metatable_register_impl_t<FUNC_CLASS_TYPE>::check_userdata_type(ls_);

        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::value();
        lua_traits_t<ARG1>::check_lua_param(ls_, ARG_INDEX(2), arg1);

        (obj_ptr->*(registed_data.dest_func))(arg1);
        return 0;
    }
};


template <typename FUNC_CLASS_TYPE, typename ARG1, typename ARG2>
struct class_function_traits_t<void (FUNC_CLASS_TYPE::*)(ARG1, ARG2)>
{
    typedef void (FUNC_CLASS_TYPE::*dest_func_t)(ARG1, ARG2);
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int lua_function(lua_State* ls_)
    {
        //! 1. check user data, if arg type check failed, reutrn arg error info to lua.
        //! 2. get the class dest, func, exe it.

        void* dest_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_register_data_t& registed_data = *((dest_register_data_t*)dest_data);

        FUNC_CLASS_TYPE* obj_ptr = metatable_register_impl_t<FUNC_CLASS_TYPE>::check_userdata_type(ls_);

        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::value();
        typename arg_traits_t<ARG2>::arg_type_t arg2 = init_value_traits_t<ARG2>::value();
        lua_traits_t<ARG1>::check_lua_param(ls_, ARG_INDEX(2), arg1);
        lua_traits_t<ARG2>::check_lua_param(ls_, ARG_INDEX(3), arg2);

        (obj_ptr->*(registed_data.dest_func))(arg1, arg2);
        return 0;
    }
};

template <typename FUNC_CLASS_TYPE, typename ARG1, typename ARG2, typename ARG3>
struct class_function_traits_t<void (FUNC_CLASS_TYPE::*)(ARG1, ARG2, ARG3)>
{
    typedef void (FUNC_CLASS_TYPE::*dest_func_t)(ARG1, ARG2, ARG3);
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int lua_function(lua_State* ls_)
    {
        //! 1. check user data, if arg type check failed, reutrn arg error info to lua.
        //! 2. get the class dest, func, exe it.

        void* dest_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_register_data_t& registed_data = *((dest_register_data_t*)dest_data);

        FUNC_CLASS_TYPE* obj_ptr = metatable_register_impl_t<FUNC_CLASS_TYPE>::check_userdata_type(ls_);

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

template <typename FUNC_CLASS_TYPE, typename ARG1, typename ARG2, typename ARG3, typename ARG4>
struct class_function_traits_t<void (FUNC_CLASS_TYPE::*)(ARG1, ARG2, ARG3, ARG4)>
{
    typedef void (FUNC_CLASS_TYPE::*dest_func_t)(ARG1, ARG2, ARG3, ARG4);
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int lua_function(lua_State* ls_)
    {
        //! 1. check user data, if arg type check failed, reutrn arg error info to lua.
        //! 2. get the class dest, func, exe it.

        void* dest_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_register_data_t& registed_data = *((dest_register_data_t*)dest_data);

        FUNC_CLASS_TYPE* obj_ptr = metatable_register_impl_t<FUNC_CLASS_TYPE>::check_userdata_type(ls_);

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

template <typename FUNC_CLASS_TYPE, typename ARG1, typename ARG2, typename ARG3, typename ARG4,
          typename ARG5>
struct class_function_traits_t<void (FUNC_CLASS_TYPE::*)(ARG1, ARG2, ARG3, ARG4, ARG5)>
{
    typedef void (FUNC_CLASS_TYPE::*dest_func_t)(ARG1, ARG2, ARG3, ARG4, ARG5);
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int lua_function(lua_State* ls_)
    {
        //! 1. check user data, if arg type check failed, reutrn arg error info to lua.
        //! 2. get the class dest, func, exe it.

        void* dest_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_register_data_t& registed_data = *((dest_register_data_t*)dest_data);

        FUNC_CLASS_TYPE* obj_ptr = metatable_register_impl_t<FUNC_CLASS_TYPE>::check_userdata_type(ls_);

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


template <typename FUNC_CLASS_TYPE, typename ARG1, typename ARG2, typename ARG3, typename ARG4,
          typename ARG5, typename ARG6>
struct class_function_traits_t<void (FUNC_CLASS_TYPE::*)(ARG1, ARG2, ARG3, ARG4, ARG5,
                                                              ARG6)>
{
    typedef void (FUNC_CLASS_TYPE::*dest_func_t)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6);
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int lua_function(lua_State* ls_)
    {
        //! 1. check user data, if arg type check failed, reutrn arg error info to lua.
        //! 2. get the class dest, func, exe it.

        void* dest_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_register_data_t& registed_data = *((dest_register_data_t*)dest_data);

        FUNC_CLASS_TYPE* obj_ptr = metatable_register_impl_t<FUNC_CLASS_TYPE>::check_userdata_type(ls_);

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

template <typename FUNC_CLASS_TYPE, typename ARG1, typename ARG2, typename ARG3, typename ARG4,
          typename ARG5, typename ARG6, typename ARG7>
struct class_function_traits_t<void (FUNC_CLASS_TYPE::*)(ARG1, ARG2, ARG3, ARG4, ARG5,
                                                              ARG6, ARG7)>
{
    typedef void (FUNC_CLASS_TYPE::*dest_func_t)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7);
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int lua_function(lua_State* ls_)
    {
        //! 1. check user data, if arg type check failed, reutrn arg error info to lua.
        //! 2. get the class dest, func, exe it.

        void* dest_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_register_data_t& registed_data = *((dest_register_data_t*)dest_data);

        FUNC_CLASS_TYPE* obj_ptr = metatable_register_impl_t<FUNC_CLASS_TYPE>::check_userdata_type(ls_);

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

template <typename FUNC_CLASS_TYPE, typename ARG1, typename ARG2, typename ARG3, typename ARG4,
          typename ARG5, typename ARG6, typename ARG7, typename ARG8>
struct class_function_traits_t<void (FUNC_CLASS_TYPE::*)(ARG1, ARG2, ARG3, ARG4, ARG5,
                                                                   ARG6, ARG7, ARG8)>
{
    typedef void (FUNC_CLASS_TYPE::*dest_func_t)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8);
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int lua_function(lua_State* ls_)
    {
        //! 1. check user data, if arg type check failed, reutrn arg error info to lua.
        //! 2. get the class dest, func, exe it.

        void* dest_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_register_data_t& registed_data = *((dest_register_data_t*)dest_data);

        FUNC_CLASS_TYPE* obj_ptr = metatable_register_impl_t<FUNC_CLASS_TYPE>::check_userdata_type(ls_);

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


template <typename FUNC_CLASS_TYPE, typename ARG1, typename ARG2, typename ARG3, typename ARG4,
          typename ARG5, typename ARG6, typename ARG7, typename ARG8, typename ARG9>
struct class_function_traits_t<void (FUNC_CLASS_TYPE::*)(ARG1, ARG2, ARG3, ARG4, ARG5,
                                                              ARG6, ARG7, ARG8, ARG9)>
{
    typedef void (FUNC_CLASS_TYPE::*dest_func_t)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8, ARG9);
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int lua_function(lua_State* ls_)
    {
        //! 1. check user data, if arg type check failed, reutrn arg error info to lua.
        //! 2. get the class dest, func, exe it.

        void* dest_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_register_data_t& registed_data = *((dest_register_data_t*)dest_data);

        FUNC_CLASS_TYPE* obj_ptr = metatable_register_impl_t<FUNC_CLASS_TYPE>::check_userdata_type(ls_);

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

template <typename FUNC_CLASS_TYPE>
struct class_function_traits_t<void (FUNC_CLASS_TYPE::*)() const>
{
    typedef void (FUNC_CLASS_TYPE::*dest_func_t)() const;
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int lua_function(lua_State* ls_)
    {
    	void* dest_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_register_data_t& registed_data = *((dest_register_data_t*)dest_data);

        FUNC_CLASS_TYPE* obj_ptr = metatable_register_impl_t<FUNC_CLASS_TYPE>::check_userdata_type(ls_);
        (obj_ptr->*(registed_data.dest_func))();
        return 0;
    }
};

template <typename FUNC_CLASS_TYPE, typename ARG1>
struct class_function_traits_t<void (FUNC_CLASS_TYPE::*)(ARG1) const>
{
    typedef void (FUNC_CLASS_TYPE::*dest_func_t)(ARG1) const;
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int lua_function(lua_State* ls_)
    {
        //! 1. check user data, if arg type check failed, reutrn arg error info to lua.
        //! 2. get the class dest, func, exe it.

        void* dest_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_register_data_t& registed_data = *((dest_register_data_t*)dest_data);

        FUNC_CLASS_TYPE* obj_ptr = metatable_register_impl_t<FUNC_CLASS_TYPE>::check_userdata_type(ls_);

        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::value();
        lua_traits_t<ARG1>::check_lua_param(ls_, ARG_INDEX(2), arg1);

        (obj_ptr->*(registed_data.dest_func))(arg1);
        return 0;
    }
};


template <typename FUNC_CLASS_TYPE, typename ARG1, typename ARG2>
struct class_function_traits_t<void (FUNC_CLASS_TYPE::*)(ARG1, ARG2) const>
{
    typedef void (FUNC_CLASS_TYPE::*dest_func_t)(ARG1, ARG2) const;
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int lua_function(lua_State* ls_)
    {
        //! 1. check user data, if arg type check failed, reutrn arg error info to lua.
        //! 2. get the class dest, func, exe it.

        void* dest_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_register_data_t& registed_data = *((dest_register_data_t*)dest_data);

        FUNC_CLASS_TYPE* obj_ptr = metatable_register_impl_t<FUNC_CLASS_TYPE>::check_userdata_type(ls_);

        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::value();
        typename arg_traits_t<ARG2>::arg_type_t arg2 = init_value_traits_t<ARG2>::value();
        lua_traits_t<ARG1>::check_lua_param(ls_, ARG_INDEX(2), arg1);
        lua_traits_t<ARG2>::check_lua_param(ls_, ARG_INDEX(3), arg2);

        (obj_ptr->*(registed_data.dest_func))(arg1, arg2);
        return 0;
    }
};

template <typename FUNC_CLASS_TYPE, typename ARG1, typename ARG2, typename ARG3>
struct class_function_traits_t<void (FUNC_CLASS_TYPE::*)(ARG1, ARG2, ARG3) const>
{
    typedef void (FUNC_CLASS_TYPE::*dest_func_t)(ARG1, ARG2, ARG3) const;
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int lua_function(lua_State* ls_)
    {
        //! 1. check user data, if arg type check failed, reutrn arg error info to lua.
        //! 2. get the class dest, func, exe it.

        void* dest_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_register_data_t& registed_data = *((dest_register_data_t*)dest_data);

        FUNC_CLASS_TYPE* obj_ptr = metatable_register_impl_t<FUNC_CLASS_TYPE>::check_userdata_type(ls_);

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

template <typename FUNC_CLASS_TYPE, typename ARG1, typename ARG2, typename ARG3, typename ARG4>
struct class_function_traits_t<void (FUNC_CLASS_TYPE::*)(ARG1, ARG2, ARG3, ARG4) const>
{
    typedef void (FUNC_CLASS_TYPE::*dest_func_t)(ARG1, ARG2, ARG3, ARG4) const;
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int lua_function(lua_State* ls_)
    {
        //! 1. check user data, if arg type check failed, reutrn arg error info to lua.
        //! 2. get the class dest, func, exe it.

        void* dest_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_register_data_t& registed_data = *((dest_register_data_t*)dest_data);

        FUNC_CLASS_TYPE* obj_ptr = metatable_register_impl_t<FUNC_CLASS_TYPE>::check_userdata_type(ls_);

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

template <typename FUNC_CLASS_TYPE, typename ARG1, typename ARG2, typename ARG3, typename ARG4,
          typename ARG5>
struct class_function_traits_t<void (FUNC_CLASS_TYPE::*)(ARG1, ARG2, ARG3, ARG4, ARG5) const>
{
    typedef void (FUNC_CLASS_TYPE::*dest_func_t)(ARG1, ARG2, ARG3, ARG4, ARG5) const;
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int lua_function(lua_State* ls_)
    {
        //! 1. check user data, if arg type check failed, reutrn arg error info to lua.
        //! 2. get the class dest, func, exe it.

        void* dest_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_register_data_t& registed_data = *((dest_register_data_t*)dest_data);

        FUNC_CLASS_TYPE* obj_ptr = metatable_register_impl_t<FUNC_CLASS_TYPE>::check_userdata_type(ls_);

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


template <typename FUNC_CLASS_TYPE, typename ARG1, typename ARG2, typename ARG3, typename ARG4,
          typename ARG5, typename ARG6>
struct class_function_traits_t<void (FUNC_CLASS_TYPE::*)(ARG1, ARG2, ARG3, ARG4, ARG5,
                                                              ARG6) const>
{
    typedef void (FUNC_CLASS_TYPE::*dest_func_t)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6) const;
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int lua_function(lua_State* ls_)
    {
        //! 1. check user data, if arg type check failed, reutrn arg error info to lua.
        //! 2. get the class dest, func, exe it.

        void* dest_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_register_data_t& registed_data = *((dest_register_data_t*)dest_data);

        FUNC_CLASS_TYPE* obj_ptr = metatable_register_impl_t<FUNC_CLASS_TYPE>::check_userdata_type(ls_);

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

template <typename FUNC_CLASS_TYPE, typename ARG1, typename ARG2, typename ARG3, typename ARG4,
          typename ARG5, typename ARG6, typename ARG7>
struct class_function_traits_t<void (FUNC_CLASS_TYPE::*)(ARG1, ARG2, ARG3, ARG4, ARG5,
                                                              ARG6, ARG7) const>
{
    typedef void (FUNC_CLASS_TYPE::*dest_func_t)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7) const;
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int lua_function(lua_State* ls_)
    {
        //! 1. check user data, if arg type check failed, reutrn arg error info to lua.
        //! 2. get the class dest, func, exe it.

        void* dest_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_register_data_t& registed_data = *((dest_register_data_t*)dest_data);

        FUNC_CLASS_TYPE* obj_ptr = metatable_register_impl_t<FUNC_CLASS_TYPE>::check_userdata_type(ls_);

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

template <typename FUNC_CLASS_TYPE, typename ARG1, typename ARG2, typename ARG3, typename ARG4,
          typename ARG5, typename ARG6, typename ARG7, typename ARG8>
struct class_function_traits_t<void (FUNC_CLASS_TYPE::*)(ARG1, ARG2, ARG3, ARG4, ARG5,
                                                                   ARG6, ARG7, ARG8) const>
{
    typedef void (FUNC_CLASS_TYPE::*dest_func_t)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8) const;
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int lua_function(lua_State* ls_)
    {
        //! 1. check user data, if arg type check failed, reutrn arg error info to lua.
        //! 2. get the class dest, func, exe it.

        void* dest_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_register_data_t& registed_data = *((dest_register_data_t*)dest_data);

        FUNC_CLASS_TYPE* obj_ptr = metatable_register_impl_t<FUNC_CLASS_TYPE>::check_userdata_type(ls_);

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


template <typename FUNC_CLASS_TYPE, typename ARG1, typename ARG2, typename ARG3, typename ARG4,
          typename ARG5, typename ARG6, typename ARG7, typename ARG8, typename ARG9>
struct class_function_traits_t<void (FUNC_CLASS_TYPE::*)(ARG1, ARG2, ARG3, ARG4, ARG5,
                                                              ARG6, ARG7, ARG8, ARG9) const>
{
    typedef void (FUNC_CLASS_TYPE::*dest_func_t)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8, ARG9) const;
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int lua_function(lua_State* ls_)
    {
        //! 1. check user data, if arg type check failed, reutrn arg error info to lua.
        //! 2. get the class dest, func, exe it.

        void* dest_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_register_data_t& registed_data = *((dest_register_data_t*)dest_data);

        FUNC_CLASS_TYPE* obj_ptr = metatable_register_impl_t<FUNC_CLASS_TYPE>::check_userdata_type(ls_);

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

template <typename FUNC_CLASS_TYPE, typename RET>
struct class_function_traits_t<RET (FUNC_CLASS_TYPE::*)()>
{
    typedef RET (FUNC_CLASS_TYPE::*dest_func_t)();
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int lua_function(lua_State* ls_)
    {
        //! 1. check user data, if arg type check failed, reutrn arg error info to lua.
        //! 2. get the class dest, func, exe it.

        void* dest_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_register_data_t& registed_data = *((dest_register_data_t*)dest_data);

        FUNC_CLASS_TYPE* obj_ptr = metatable_register_impl_t<FUNC_CLASS_TYPE>::check_userdata_type(ls_);
        RET ret = (obj_ptr->*(registed_data.dest_func))();
        lua_traits_t<RET>::push_stack(ls_, ret);
        return 1;
    }
};


template <typename FUNC_CLASS_TYPE, typename ARG1, typename RET>
struct class_function_traits_t<RET (FUNC_CLASS_TYPE::*)(ARG1)>
{
    typedef RET (FUNC_CLASS_TYPE::*dest_func_t)(ARG1);
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int lua_function(lua_State* ls_)
    {
        //! 1. check user data, if arg type check failed, reutrn arg error info to lua.
        //! 2. get the class dest, func, exe it.

        void* dest_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_register_data_t& registed_data = *((dest_register_data_t*)dest_data);

        FUNC_CLASS_TYPE* obj_ptr = metatable_register_impl_t<FUNC_CLASS_TYPE>::check_userdata_type(ls_);

        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::value();
        lua_traits_t<ARG1>::check_lua_param(ls_, ARG_INDEX(2), arg1);

        RET ret = (obj_ptr->*(registed_data.dest_func))(arg1);
        lua_traits_t<RET>::push_stack(ls_, ret);
        return 1;
    }
};

template <typename FUNC_CLASS_TYPE, typename ARG1, typename ARG2,
          typename RET>
struct class_function_traits_t<RET (FUNC_CLASS_TYPE::*)(ARG1, ARG2)>
{
    typedef RET (FUNC_CLASS_TYPE::*dest_func_t)(ARG1, ARG2);
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int lua_function(lua_State* ls_)
    {
        //! 1. check user data, if arg type check failed, reutrn arg error info to lua.
        //! 2. get the class dest, func, exe it.

        void* dest_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_register_data_t& registed_data = *((dest_register_data_t*)dest_data);

        FUNC_CLASS_TYPE* obj_ptr = metatable_register_impl_t<FUNC_CLASS_TYPE>::check_userdata_type(ls_);

        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::value();
        typename arg_traits_t<ARG2>::arg_type_t arg2 = init_value_traits_t<ARG2>::value();
        lua_traits_t<ARG1>::check_lua_param(ls_, ARG_INDEX(2), arg1);
        lua_traits_t<ARG2>::check_lua_param(ls_, ARG_INDEX(3), arg2);

        RET ret = (obj_ptr->*(registed_data.dest_func))(arg1, arg2);
        lua_traits_t<RET>::push_stack(ls_, ret);
        return 1;
    }
};

template <typename FUNC_CLASS_TYPE, typename ARG1, typename ARG2, typename ARG3,
          typename RET>
struct class_function_traits_t<RET (FUNC_CLASS_TYPE::*)(ARG1, ARG2, ARG3)>
{
    typedef RET (FUNC_CLASS_TYPE::*dest_func_t)(ARG1, ARG2, ARG3);
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int lua_function(lua_State* ls_)
    {
        //! 1. check user data, if arg type check failed, reutrn arg error info to lua.
        //! 2. get the class dest, func, exe it.

        void* dest_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_register_data_t& registed_data = *((dest_register_data_t*)dest_data);

        FUNC_CLASS_TYPE* obj_ptr = metatable_register_impl_t<FUNC_CLASS_TYPE>::check_userdata_type(ls_);

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

template <typename FUNC_CLASS_TYPE, typename ARG1, typename ARG2, typename ARG3, typename ARG4,
          typename RET>
struct class_function_traits_t<RET (FUNC_CLASS_TYPE::*)(ARG1, ARG2, ARG3, ARG4)>
{
    typedef RET (FUNC_CLASS_TYPE::*dest_func_t)(ARG1, ARG2, ARG3, ARG4);
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int lua_function(lua_State* ls_)
    {
        //! 1. check user data, if arg type check failed, reutrn arg error info to lua.
        //! 2. get the class dest, func, exe it.

        void* dest_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_register_data_t& registed_data = *((dest_register_data_t*)dest_data);

        FUNC_CLASS_TYPE* obj_ptr = metatable_register_impl_t<FUNC_CLASS_TYPE>::check_userdata_type(ls_);

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

template <typename FUNC_CLASS_TYPE, typename ARG1, typename ARG2, typename ARG3, typename ARG4,
          typename ARG5, typename RET>
struct class_function_traits_t<RET (FUNC_CLASS_TYPE::*)(ARG1, ARG2, ARG3, ARG4, ARG5)>
{
    typedef RET (FUNC_CLASS_TYPE::*dest_func_t)(ARG1, ARG2, ARG3, ARG4, ARG5);
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int lua_function(lua_State* ls_)
    {
        //! 1. check user data, if arg type check failed, reutrn arg error info to lua.
        //! 2. get the class dest, func, exe it.

        void* dest_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_register_data_t& registed_data = *((dest_register_data_t*)dest_data);

        FUNC_CLASS_TYPE* obj_ptr = metatable_register_impl_t<FUNC_CLASS_TYPE>::check_userdata_type(ls_);

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


template <typename FUNC_CLASS_TYPE, typename ARG1, typename ARG2, typename ARG3, typename ARG4,
          typename ARG5, typename ARG6, typename RET>
struct class_function_traits_t<RET (FUNC_CLASS_TYPE::*)(ARG1, ARG2, ARG3, ARG4, ARG5,
                                                              ARG6)>
{
    typedef RET (FUNC_CLASS_TYPE::*dest_func_t)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6);
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int lua_function(lua_State* ls_)
    {
        //! 1. check user data, if arg type check failed, reutrn arg error info to lua.
        //! 2. get the class dest, func, exe it.

        void* dest_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_register_data_t& registed_data = *((dest_register_data_t*)dest_data);

        FUNC_CLASS_TYPE* obj_ptr = metatable_register_impl_t<FUNC_CLASS_TYPE>::check_userdata_type(ls_);

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

template <typename FUNC_CLASS_TYPE, typename ARG1, typename ARG2, typename ARG3, typename ARG4,
          typename ARG5, typename ARG6, typename ARG7, typename RET>
struct class_function_traits_t<RET (FUNC_CLASS_TYPE::*)(ARG1, ARG2, ARG3, ARG4, ARG5,
                                                              ARG6, ARG7)>
{
    typedef RET (FUNC_CLASS_TYPE::*dest_func_t)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7);
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int lua_function(lua_State* ls_)
    {
        //! 1. check user data, if arg type check failed, reutrn arg error info to lua.
        //! 2. get the class dest, func, exe it.

        void* dest_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_register_data_t& registed_data = *((dest_register_data_t*)dest_data);

        FUNC_CLASS_TYPE* obj_ptr = metatable_register_impl_t<FUNC_CLASS_TYPE>::check_userdata_type(ls_);

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

template <typename FUNC_CLASS_TYPE, typename ARG1, typename ARG2, typename ARG3, typename ARG4,
          typename ARG5, typename ARG6, typename ARG7, typename ARG8, typename RET>
struct class_function_traits_t<RET (FUNC_CLASS_TYPE::*)(ARG1, ARG2, ARG3, ARG4, ARG5,
                                                                   ARG6, ARG7, ARG8)>
{
    typedef RET (FUNC_CLASS_TYPE::*dest_func_t)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8);
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int lua_function(lua_State* ls_)
    {
        //! 1. check user data, if arg type check failed, reutrn arg error info to lua.
        //! 2. get the class dest, func, exe it.

        void* dest_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_register_data_t& registed_data = *((dest_register_data_t*)dest_data);

        FUNC_CLASS_TYPE* obj_ptr = metatable_register_impl_t<FUNC_CLASS_TYPE>::check_userdata_type(ls_);

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


template <typename FUNC_CLASS_TYPE, typename ARG1, typename ARG2, typename ARG3, typename ARG4,
          typename ARG5, typename ARG6, typename ARG7, typename ARG8, typename ARG9, typename RET>
struct class_function_traits_t<RET (FUNC_CLASS_TYPE::*)(ARG1, ARG2, ARG3, ARG4, ARG5,
                                                              ARG6, ARG7, ARG8, ARG9)>
{
    typedef RET (FUNC_CLASS_TYPE::*dest_func_t)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8, ARG9);
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int lua_function(lua_State* ls_)
    {
        //! 1. check user data, if arg type check failed, reutrn arg error info to lua.
        //! 2. get the class dest, func, exe it.

        void* dest_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_register_data_t& registed_data = *((dest_register_data_t*)dest_data);

        FUNC_CLASS_TYPE* obj_ptr = metatable_register_impl_t<FUNC_CLASS_TYPE>::check_userdata_type(ls_);

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


template <typename FUNC_CLASS_TYPE, typename RET>
struct class_function_traits_t<RET (FUNC_CLASS_TYPE::*)() const>
{
    typedef RET (FUNC_CLASS_TYPE::*dest_func_t)() const;
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int lua_function(lua_State* ls_)
    {
        //! 1. check user data, if arg type check failed, reutrn arg error info to lua.
        //! 2. get the class dest, func, exe it.

        void* dest_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_register_data_t& registed_data = *((dest_register_data_t*)dest_data);

        FUNC_CLASS_TYPE* obj_ptr = metatable_register_impl_t<FUNC_CLASS_TYPE>::check_userdata_type(ls_);
        RET ret = (obj_ptr->*(registed_data.dest_func))();
        lua_traits_t<RET>::push_stack(ls_, ret);
        return 1;
    }
};


template <typename FUNC_CLASS_TYPE, typename ARG1, typename RET>
struct class_function_traits_t<RET (FUNC_CLASS_TYPE::*)(ARG1) const>
{
    typedef RET (FUNC_CLASS_TYPE::*dest_func_t)(ARG1) const;
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int lua_function(lua_State* ls_)
    {
        //! 1. check user data, if arg type check failed, reutrn arg error info to lua.
        //! 2. get the class dest, func, exe it.

        void* dest_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_register_data_t& registed_data = *((dest_register_data_t*)dest_data);

        FUNC_CLASS_TYPE* obj_ptr = metatable_register_impl_t<FUNC_CLASS_TYPE>::check_userdata_type(ls_);

        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::value();
        lua_traits_t<ARG1>::check_lua_param(ls_, ARG_INDEX(2), arg1);

        RET ret = (obj_ptr->*(registed_data.dest_func))(arg1);
        lua_traits_t<RET>::push_stack(ls_, ret);
        return 1;
    }
};

template <typename FUNC_CLASS_TYPE, typename ARG1, typename ARG2,
          typename RET>
struct class_function_traits_t<RET (FUNC_CLASS_TYPE::*)(ARG1, ARG2) const>
{
    typedef RET (FUNC_CLASS_TYPE::*dest_func_t)(ARG1, ARG2) const;
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int lua_function(lua_State* ls_)
    {
        //! 1. check user data, if arg type check failed, reutrn arg error info to lua.
        //! 2. get the class dest, func, exe it.

        void* dest_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_register_data_t& registed_data = *((dest_register_data_t*)dest_data);

        FUNC_CLASS_TYPE* obj_ptr = metatable_register_impl_t<FUNC_CLASS_TYPE>::check_userdata_type(ls_);

        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::value();
        typename arg_traits_t<ARG2>::arg_type_t arg2 = init_value_traits_t<ARG2>::value();
        lua_traits_t<ARG1>::check_lua_param(ls_, ARG_INDEX(2), arg1);
        lua_traits_t<ARG2>::check_lua_param(ls_, ARG_INDEX(3), arg2);

        RET ret = (obj_ptr->*(registed_data.dest_func))(arg1, arg2);
        lua_traits_t<RET>::push_stack(ls_, ret);
        return 1;
    }
};

template <typename FUNC_CLASS_TYPE, typename ARG1, typename ARG2, typename ARG3,
          typename RET>
struct class_function_traits_t<RET (FUNC_CLASS_TYPE::*)(ARG1, ARG2, ARG3) const>
{
    typedef RET (FUNC_CLASS_TYPE::*dest_func_t)(ARG1, ARG2, ARG3) const;
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int lua_function(lua_State* ls_)
    {
        //! 1. check user data, if arg type check failed, reutrn arg error info to lua.
        //! 2. get the class dest, func, exe it.

        void* dest_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_register_data_t& registed_data = *((dest_register_data_t*)dest_data);

        FUNC_CLASS_TYPE* obj_ptr = metatable_register_impl_t<FUNC_CLASS_TYPE>::check_userdata_type(ls_);

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

template <typename FUNC_CLASS_TYPE, typename ARG1, typename ARG2, typename ARG3, typename ARG4,
          typename RET>
struct class_function_traits_t<RET (FUNC_CLASS_TYPE::*)(ARG1, ARG2, ARG3, ARG4) const>
{
    typedef RET (FUNC_CLASS_TYPE::*dest_func_t)(ARG1, ARG2, ARG3, ARG4) const;
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int lua_function(lua_State* ls_)
    {
        //! 1. check user data, if arg type check failed, reutrn arg error info to lua.
        //! 2. get the class dest, func, exe it.

        void* dest_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_register_data_t& registed_data = *((dest_register_data_t*)dest_data);

        FUNC_CLASS_TYPE* obj_ptr = metatable_register_impl_t<FUNC_CLASS_TYPE>::check_userdata_type(ls_);

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

template <typename FUNC_CLASS_TYPE, typename ARG1, typename ARG2, typename ARG3, typename ARG4,
          typename ARG5, typename RET>
struct class_function_traits_t<RET (FUNC_CLASS_TYPE::*)(ARG1, ARG2, ARG3, ARG4, ARG5) const>
{
    typedef RET (FUNC_CLASS_TYPE::*dest_func_t)(ARG1, ARG2, ARG3, ARG4, ARG5) const;
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int lua_function(lua_State* ls_)
    {
        //! 1. check user data, if arg type check failed, reutrn arg error info to lua.
        //! 2. get the class dest, func, exe it.

        void* dest_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_register_data_t& registed_data = *((dest_register_data_t*)dest_data);

        FUNC_CLASS_TYPE* obj_ptr = metatable_register_impl_t<FUNC_CLASS_TYPE>::check_userdata_type(ls_);

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


template <typename FUNC_CLASS_TYPE, typename ARG1, typename ARG2, typename ARG3, typename ARG4,
          typename ARG5, typename ARG6, typename RET>
struct class_function_traits_t<RET (FUNC_CLASS_TYPE::*)(ARG1, ARG2, ARG3, ARG4, ARG5,
                                                              ARG6) const>
{
    typedef RET (FUNC_CLASS_TYPE::*dest_func_t)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6) const;
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int lua_function(lua_State* ls_)
    {
        //! 1. check user data, if arg type check failed, reutrn arg error info to lua.
        //! 2. get the class dest, func, exe it.

        void* dest_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_register_data_t& registed_data = *((dest_register_data_t*)dest_data);

        FUNC_CLASS_TYPE* obj_ptr = metatable_register_impl_t<FUNC_CLASS_TYPE>::check_userdata_type(ls_);

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

template <typename FUNC_CLASS_TYPE, typename ARG1, typename ARG2, typename ARG3, typename ARG4,
          typename ARG5, typename ARG6, typename ARG7, typename RET>
struct class_function_traits_t<RET (FUNC_CLASS_TYPE::*)(ARG1, ARG2, ARG3, ARG4, ARG5,
                                                              ARG6, ARG7) const>
{
    typedef RET (FUNC_CLASS_TYPE::*dest_func_t)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7) const;
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int lua_function(lua_State* ls_)
    {
        //! 1. check user data, if arg type check failed, reutrn arg error info to lua.
        //! 2. get the class dest, func, exe it.

        void* dest_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_register_data_t& registed_data = *((dest_register_data_t*)dest_data);

        FUNC_CLASS_TYPE* obj_ptr = metatable_register_impl_t<FUNC_CLASS_TYPE>::check_userdata_type(ls_);

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

template <typename FUNC_CLASS_TYPE, typename ARG1, typename ARG2, typename ARG3, typename ARG4,
          typename ARG5, typename ARG6, typename ARG7, typename ARG8, typename RET>
struct class_function_traits_t<RET (FUNC_CLASS_TYPE::*)(ARG1, ARG2, ARG3, ARG4, ARG5,
                                                                   ARG6, ARG7, ARG8) const>
{
    typedef RET (FUNC_CLASS_TYPE::*dest_func_t)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8) const;
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int lua_function(lua_State* ls_)
    {
        //! 1. check user data, if arg type check failed, reutrn arg error info to lua.
        //! 2. get the class dest, func, exe it.

        void* dest_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_register_data_t& registed_data = *((dest_register_data_t*)dest_data);

        FUNC_CLASS_TYPE* obj_ptr = metatable_register_impl_t<FUNC_CLASS_TYPE>::check_userdata_type(ls_);

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


template <typename FUNC_CLASS_TYPE, typename ARG1, typename ARG2, typename ARG3, typename ARG4,
          typename ARG5, typename ARG6, typename ARG7, typename ARG8, typename ARG9, typename RET>
struct class_function_traits_t<RET (FUNC_CLASS_TYPE::*)(ARG1, ARG2, ARG3, ARG4, ARG5,
                                                              ARG6, ARG7, ARG8, ARG9) const>
{
    typedef RET (FUNC_CLASS_TYPE::*dest_func_t)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8, ARG9) const;
    typedef register_data_t<dest_func_t>        dest_register_data_t;

    static  int lua_function(lua_State* ls_)
    {
        //! 1. check user data, if arg type check failed, reutrn arg error info to lua.
        //! 2. get the class dest, func, exe it.

        void* dest_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_register_data_t& registed_data = *((dest_register_data_t*)dest_data);

        FUNC_CLASS_TYPE* obj_ptr = metatable_register_impl_t<FUNC_CLASS_TYPE>::check_userdata_type(ls_);

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

template <typename CLASS_TYPE, typename RET>
struct class_property_traits_t
{
	typedef RET property_t;
	typedef RET CLASS_TYPE::* property_ptr_t;
	static int process_index(lua_State* ls_, void* field_info_, const char* key_)
	{
		typedef property_registry_t<property_ptr_t> property_registry_t;
		CLASS_TYPE* obj_ptr = metatable_register_impl_t<CLASS_TYPE>::check_userdata_type(ls_);

		property_registry_t* reg = (property_registry_t*)field_info_;
		property_ptr_t ptr = reg->field_ptr;

		if (ptr)
		{
			lua_traits_t<property_t>::push_stack(ls_, (obj_ptr->*ptr));
			return 1;
		}
		else
		{
			printf("none this field<%s>\n", key_);
			return 0;
		}

		return 0;
	}

	static int process_newindex(lua_State* ls_, void* field_info_, const char* key_, int value_index_)
	{
		typedef property_registry_t<property_ptr_t> property_registry_t;
		CLASS_TYPE* obj_ptr = metatable_register_impl_t<CLASS_TYPE>::check_userdata_type(ls_);

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
			printf("none this field<%s>\n", key_);
			return 0;
		}

		return 0;
	}
};

template<>
struct function_traits_t<void(*)()>
{
    typedef void (*dest_func_t)();
    static  int lua_function(lua_State* ls_)
    {
        void* user_data = lua_touserdata (ls_, lua_upvalueindex(1));

        dest_func_t& registed_func = *((dest_func_t*)user_data);
        registed_func();
        return 0;
    }
};


template <typename ARG1>
struct function_traits_t<void(*)(ARG1)>
{
    typedef void (*dest_func_t)(ARG1);
    static  int lua_function(lua_State* ls_)
    {
        typename arg_traits_t<ARG1>::arg_type_t arg1 = init_value_traits_t<ARG1>::init_value();

        lua_traits_t<ARG1>::check_lua_param(ls_, LUA_ARG_INDEX(1), arg1);

        void* user_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_func_t& registed_func = *((dest_func_t*)user_data);

        registed_func(arg1);
        return 0;
    }
};

template <typename ARG1, typename ARG2>
struct function_traits_t<void(*)(ARG1, ARG2)>
{
    typedef void (*dest_func_t)(ARG1, ARG2);
    static  int lua_function(lua_State* ls_)
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

template <typename ARG1, typename ARG2, typename ARG3>
struct function_traits_t<void(*)(ARG1, ARG2, ARG3)>
{
    typedef void (*dest_func_t)(ARG1, ARG2, ARG3);
    static  int lua_function(lua_State* ls_)
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

template <typename ARG1, typename ARG2, typename ARG3, typename ARG4>
struct function_traits_t<void (*)(ARG1, ARG2, ARG3, ARG4)>
{
    typedef void (*dest_func_t)(ARG1, ARG2, ARG3, ARG4);
    static  int lua_function(lua_State* ls_)
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

template <typename ARG1, typename ARG2, typename ARG3, typename ARG4,
          typename ARG5>
struct function_traits_t<void (*)(ARG1, ARG2, ARG3, ARG4, ARG5)>
{
    typedef void (*dest_func_t)(ARG1, ARG2, ARG3, ARG4, ARG5);
    static  int lua_function(lua_State* ls_)
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
template <typename ARG1, typename ARG2, typename ARG3, typename ARG4,
          typename ARG5, typename ARG6>
struct function_traits_t<void (*)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6)>
{
    typedef void (*dest_func_t)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6);
    static  int lua_function(lua_State* ls_)
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

template <typename ARG1, typename ARG2, typename ARG3, typename ARG4,
          typename ARG5, typename ARG6, typename ARG7>
struct function_traits_t<void (*)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7)>
{
    typedef void (*dest_func_t)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7);
    static  int lua_function(lua_State* ls_)
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

template <typename ARG1, typename ARG2, typename ARG3, typename ARG4,
          typename ARG5, typename ARG6, typename ARG7, typename ARG8>
struct function_traits_t<void (*)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8)>
{
    typedef void (*dest_func_t)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8);
    static  int lua_function(lua_State* ls_)
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


template <typename ARG1, typename ARG2, typename ARG3, typename ARG4,
          typename ARG5, typename ARG6, typename ARG7, typename ARG8, typename ARG9>
struct function_traits_t<void (*)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8, ARG9)>
{
    typedef void (*dest_func_t)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8, ARG9);
    static  int lua_function(lua_State* ls_)
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
struct function_traits_t<RET (*)()>
{
    typedef RET (*dest_func_t)();
    static  int lua_function(lua_State* ls_)
    {
        void* user_data = lua_touserdata (ls_, lua_upvalueindex(1));
        dest_func_t& registed_func = *((dest_func_t*)user_data);

        RET ret = registed_func();
        lua_traits_t<RET>::push_stack(ls_, ret);

        return 1;
    }
};

template <typename RET, typename ARG1>
struct function_traits_t<RET (*)(ARG1)>
{
    typedef RET (*dest_func_t)(ARG1);
    static  int lua_function(lua_State* ls_)
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

template <typename RET, typename ARG1, typename ARG2>
struct function_traits_t<RET (*)(ARG1, ARG2)>
{
    typedef RET (*dest_func_t)(ARG1, ARG2);
    static  int lua_function(lua_State* ls_)
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

template <typename RET, typename ARG1, typename ARG2, typename ARG3>
struct function_traits_t<RET (*)(ARG1, ARG2, ARG3)>
{
    typedef RET (*dest_func_t)(ARG1, ARG2, ARG3);
    static  int lua_function(lua_State* ls_)
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

template <typename RET, typename ARG1, typename ARG2, typename ARG3, typename ARG4>
struct function_traits_t<RET (*)(ARG1, ARG2, ARG3, ARG4)>
{
    typedef RET (*dest_func_t)(ARG1, ARG2, ARG3, ARG4);
    static  int lua_function(lua_State* ls_)
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

template <typename RET, typename ARG1, typename ARG2, typename ARG3, typename ARG4,
          typename ARG5>
struct function_traits_t<RET (*)(ARG1, ARG2, ARG3, ARG4, ARG5)>
{
    typedef RET (*dest_func_t)(ARG1, ARG2, ARG3, ARG4, ARG5);
    static  int lua_function(lua_State* ls_)
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


template <typename RET, typename ARG1, typename ARG2, typename ARG3, typename ARG4,
          typename ARG5, typename ARG6>
struct function_traits_t<RET (*)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6)>
{
    typedef RET (*dest_func_t)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6);
    static  int lua_function(lua_State* ls_)
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

template <typename RET, typename ARG1, typename ARG2, typename ARG3, typename ARG4,
          typename ARG5, typename ARG6, typename ARG7>
struct function_traits_t<RET (*)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7)>
{
    typedef RET (*dest_func_t)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7);
    static  int lua_function(lua_State* ls_)
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

template <typename RET, typename ARG1, typename ARG2, typename ARG3, typename ARG4,
          typename ARG5, typename ARG6, typename ARG7, typename ARG8>
struct function_traits_t<RET (*)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8)>
{
    typedef RET (*dest_func_t)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8);
    static  int lua_function(lua_State* ls_)
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

template <typename RET, typename ARG1, typename ARG2, typename ARG3, typename ARG4,
          typename ARG5, typename ARG6, typename ARG7, typename ARG8, typename ARG9>
struct function_traits_t<RET (*)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8, ARG9)>
{
    typedef RET (*dest_func_t)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8, ARG9);
    static  int lua_function(lua_State* ls_)
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

}

#endif

