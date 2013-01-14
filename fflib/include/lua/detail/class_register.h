#ifndef _CLASS_REGISTER_H_
#define _CLASS_REGISTER_H_

#include <string>
#include <iostream>
using namespace std;

#include "lua_type.h"
#include "class_register_impl.h"

#define METATABLE_NAME(x) metatable_name((x)).c_str()

class class_register_t
{
public:
    template<typename CLASS_TYPE, typename FUNC_TYPE>
    static int register_class_base(lua_State* ls_, const string& class_name_);
    template<typename CLASS_TYPE, typename FUNC_TYPE>
    static int register_virtual_class_base(lua_State* ls_, const string& class_name_);
    template<typename CLASS_TYPE, typename FUNC_TYPE>
    static int register_class_method(lua_State* ls_, const string& class_name, const string& func_name, FUNC_TYPE func_);

    template<typename CLASS_TYPE, typename PROPERTY_TYPE>
    static int register_class_property(lua_State* ls_, const string& property_name_, PROPERTY_TYPE p_);

private:
    static string metatable_name(const string& name_)
    {
        string ret = "fflua." + name_;
        return ret;
    }

    template<typename CLASS_TYPE>
    static int index_function(lua_State* ls_)
    {
        string key;
        lua_traits_t<string>::check_lua_param(ls_, ARG_INDEX(2), key);

        luaL_getmetatable(ls_, lua_type_registry_t<CLASS_TYPE>::get_type_name());
        int mt_index = lua_gettop(ls_);

        lua_getfield(ls_, -1, key.c_str());
        lua_remove(ls_, mt_index);

        if (lua_isuserdata(ls_, -1))
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

    template<typename CLASS_TYPE>
    static int newindex_function(lua_State* ls_)
    {
        string key;
        lua_traits_t<string>::check_lua_param(ls_, ARG_INDEX(2), key);

        luaL_getmetatable(ls_, lua_type_registry_t<CLASS_TYPE>::get_type_name());
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

    template <typename CLASS_TYPE>
    static int get_pointer(lua_State* ls_)
    {
        CLASS_TYPE** obj_ptr = get_userdata_ref<CLASS_TYPE>(ls_);
        long  addr = long(*obj_ptr);
        lua_traits_t<long>::push_stack(ls_, addr);
        return 1;
    }
};

template<typename CLASS_TYPE, typename FUNC_TYPE>
int class_register_t::register_virtual_class_base(lua_State* ls_, const string& class_name_)
{
    //! 1. create a metatable, this class will use this metatable.
    //! 2. set metatable.__index = metatable
    //! create a table for class.
    //! set table.new = new_function
    //! set metatable.delete = delete_function

    lua_type_registry_t<CLASS_TYPE>::set_type_name(METATABLE_NAME(class_name_));

    luaL_newmetatable(ls_, METATABLE_NAME(class_name_));
    int  metatable_index = lua_gettop(ls_);

    lua_pushstring(ls_, "__index");
    lua_function_t index_function = &class_register_t::index_function<CLASS_TYPE>;
    lua_pushcclosure(ls_, index_function, 0);
    lua_settable(ls_, -3);

    lua_pushstring(ls_, "get_pointer");
    lua_function_t pointer_function = &class_register_t::get_pointer<CLASS_TYPE>;
    lua_pushcclosure(ls_, pointer_function, 0);
    lua_settable(ls_, -3);

    //! add function to check ptr whether null or not
    lua_pushstring(ls_, "__newindex");
    lua_function_t newindex_function = &class_register_t::newindex_function<CLASS_TYPE>;
    lua_pushcclosure(ls_, newindex_function, 0);
    lua_settable(ls_, -3);

    //! typedef class_register_impl_t<lua_class_instance_t, pair<CLASS_TYPE, FUNC_TYPE> >   register_impl_t;
    //! typedef typename register_impl_t::dest_register_data_t                              register_data_t;

    //! lua_function_t closure_func = &register_impl_t::closure_function;
    //! lua_pushcclosure(ls_, closure_func, 0);

    lua_newtable(ls_);
    //! lua_pushstring(ls_, "new");
    //! lua_pushvalue(ls_, -3);//! tb.new = closure_func
    //! lua_settable(ls_, -3);

    //! now table created by lua_newtable at pos -1
    lua_setglobal(ls_, class_name_.c_str());
    //! now closure_func at pos -1
    //! lua_pop(ls_, 1);

    //! begin to set metatable delete function.
    typedef class_register_impl_t<lua_class_delete_t, CLASS_TYPE>   delete_register_impl_t;
    //! typedef typename register_impl_t::dest_register_data_t          delete_register_data_t;

    lua_function_t delete_closure_func = &delete_register_impl_t::closure_function;
    lua_pushcclosure(ls_, delete_closure_func, 0);

    lua_pushstring(ls_, "delete");
    lua_pushvalue(ls_, -2);
    lua_settable(ls_, metatable_index);
    //! now closure_func at pos -1
    //! metatable at pos -2
    lua_pop(ls_, 2);

    return 0;
}


template<typename CLASS_TYPE, typename FUNC_TYPE>
int class_register_t::register_class_base(lua_State* ls_, const string& class_name_)
{
    //! 1. create a metatable, this class will use this metatable.
    //! 2. set metatable.__index = metatable
    //! create a table for class.
    //! set table.new = new_function
    //! set metatable.delete = delete_function

    lua_type_registry_t<CLASS_TYPE>::set_type_name(METATABLE_NAME(class_name_));

    luaL_newmetatable(ls_, METATABLE_NAME(class_name_));
    int  metatable_index = lua_gettop(ls_);

    lua_pushstring(ls_, "__index");
    lua_function_t index_function = &class_register_t::index_function<CLASS_TYPE>;
    lua_pushcclosure(ls_, index_function, 0);
    lua_settable(ls_, -3);

    lua_pushstring(ls_, "get_pointer");
    lua_function_t pointer_function = &class_register_t::get_pointer<CLASS_TYPE>;
    lua_pushcclosure(ls_, pointer_function, 0);
    lua_settable(ls_, -3);

    //! add function to check ptr whether null or not
    lua_pushstring(ls_, "__newindex");
    lua_function_t newindex_function = &class_register_t::newindex_function<CLASS_TYPE>;
    lua_pushcclosure(ls_, newindex_function, 0);
    lua_settable(ls_, -3);

    typedef class_register_impl_t<lua_class_instance_t, pair<CLASS_TYPE, FUNC_TYPE> >   register_impl_t;
    typedef typename register_impl_t::dest_register_data_t                              register_data_t;

    lua_function_t closure_func = &register_impl_t::closure_function;
    lua_pushcclosure(ls_, closure_func, 0);

    lua_newtable(ls_);
    lua_pushstring(ls_, "new");
    lua_pushvalue(ls_, -3);//! tb.new = closure_func
    lua_settable(ls_, -3);

    //! now table created by lua_newtable at pos -1
    lua_setglobal(ls_, class_name_.c_str());
    //! now closure_func at pos -1
    lua_pop(ls_, 1);

    //! begin to set metatable delete function.
    typedef class_register_impl_t<lua_class_delete_t, CLASS_TYPE>   delete_register_impl_t;
    typedef typename register_impl_t::dest_register_data_t          delete_register_data_t;

    lua_function_t delete_closure_func = &delete_register_impl_t::closure_function;
    lua_pushcclosure(ls_, delete_closure_func, 0);

    lua_pushstring(ls_, "delete");
    lua_pushvalue(ls_, -2);
    lua_settable(ls_, metatable_index);
    //! now closure_func at pos -1
    //! metatable at pos -2
    lua_pop(ls_, 2);

    return 0;
}

template<typename CLASS_TYPE, typename FUNC_TYPE>
int class_register_t::register_class_method(lua_State* ls_, const string& class_name_, const string& func_name_, FUNC_TYPE func_)
{
    typedef typename class_register_impl_t<CLASS_TYPE, FUNC_TYPE>::dest_register_data_t register_data_t;

    //! 1. register method, set a userdata to closure function.
    //! 2. set method to metatable filed
    //! 3. clear the stack

    lua_function_t closure_func = &class_register_impl_t<CLASS_TYPE, FUNC_TYPE>::closure_function;

    void* user_data_ptr = lua_newuserdata(ls_, sizeof(register_data_t));
    new(user_data_ptr) register_data_t(func_);
    lua_pushcclosure(ls_, closure_func, 1);

    luaL_getmetatable(ls_, METATABLE_NAME(class_name_));
    lua_pushstring(ls_, func_name_.c_str());
    lua_pushvalue(ls_, -3);
    lua_settable(ls_, -3);

    //! now, metatalbe and closure_func at stack pos -1
    lua_pop(ls_, 2);

    return 0;
}

template<typename CLASS_TYPE, typename PROPERTY_TYPE>
int class_register_t::register_class_property(lua_State* ls_, const string& property_name_, PROPERTY_TYPE p_)
{
    index_impl_func_t index_func = &class_register_impl_t<pair<CLASS_TYPE, lua_class_property_t>,
                                                         PROPERTY_TYPE>::index_impl;
    newindex_impl_func_t newindex_func = &class_register_impl_t<pair<CLASS_TYPE, lua_class_property_t>,
                                                         PROPERTY_TYPE>::newindex_impl;

    typedef lua_property_udata_t<PROPERTY_TYPE> reg_property_udata_t;

    reg_property_udata_t* ptr_udata = (reg_property_udata_t*)lua_newuserdata(ls_, sizeof(reg_property_udata_t));
    ptr_udata->reg_property.field_ptr = p_;

    int udata_index = lua_gettop(ls_);
    ptr_udata->index_impl_func = index_func;
    ptr_udata->newindex_impl_func = newindex_func;
    ptr_udata->field_ptr = (void*)(&(ptr_udata->reg_property));

    luaL_getmetatable(ls_, lua_type_registry_t<CLASS_TYPE>::get_type_name());
    lua_pushstring(ls_, property_name_.c_str());
    lua_pushvalue(ls_, udata_index);
    lua_settable(ls_, -3);

    lua_pop(ls_, 1);
    lua_remove(ls_, udata_index);

    return 0;
}

#endif
