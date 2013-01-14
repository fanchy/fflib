#ifndef _LUA_MACRO_H_
#define _LUA_MACRO_H_

//!  --------------------------------------------------- lua register macro for c++ class register.
#define LUA_REGISTER_BEGIN(cpp_register) \
    static void cpp_register(fflua_t& handler_) {\

#define REGISTER_GLOBAL_FUNCTION(func_name_, func_) \
    handler_.register_global_function(func_name_, func_);

#define REGISTER_STATIC_FUNCTION(table_name_, func_name_, func_) \
    handler_.register_static_function(table_name_, func_name_, func_);

#define REGISTER_CLASS_BASE(class_name_, class_type_, construct_type_) \
    handler_.register_class_base<class_type_, construct_type_ >(class_name_);

#define REGISTER_VIRTUAL_CLASS_BASE(class_name_, class_type_, construct_type_) \
    handler_.register_virtual_class_base<class_type_, construct_type_ >(class_name_);

#define REGISTER_CLASS_METHOD(class_name_, func_name_, class_type_, func_) \
    handler_.register_class_method<class_type_>(class_name_, func_name_, func_);

#define REGISTER_CLASS_PROPERTY(class_type_, property_name_, field_) \
    handler_.register_class_property<class_type_>(property_name_, field_);

#define LUA_REGISTER_END }

//! --------------------------------------------------- these macro ueed to generate lua dynamic link library
#define DEFINE_LUA_EXT_BEGIN(x) extern "C" { int LUA_API luaopen_##x (lua_State* ls_) { fflua_t fm_lua(ls_);
#define DEFINE_LUA_INTERFACE(x)  fm_lua.multi_register(x);
#define DEFINE_LUA_EXT_END return 0; }}

#endif
