#ifndef _LUA_TYPE_H_
#define _LUA_TYPE_H_

#include <stdint.h>
#include <stdlib.h>
#include <lua.hpp>

#include <stdexcept>
#include <string>
#include <map>
using namespace std;

struct lua_string_tool_t
{
	inline static const char* c_str(const string& s_) { return s_.c_str(); }
	inline static const char* c_str(const char* s_)   { return s_; }
};

class lua_exception_t: public exception
{
public:
    explicit lua_exception_t(const char* err_);
    explicit lua_exception_t(const string& err_);
    ~lua_exception_t() throw ();

    const char* what() const throw ();
private:
    string m_err;
};

class lua_helper
{
public:
    static void stack_dump(lua_State* ls_);
    static string dump_error(lua_State* ls_, const char *fmt, ...);
};

typedef int (*lua_function_t) (lua_State *L);

class lua_nil_t{};
class lua_class_instance_t{};
class lua_class_delete_t{};
class lua_class_property_t{};

template<typename T>
struct property_registry_t
{
    property_registry_t():field_ptr(NULL)
    {
    }
    T field_ptr;
};

template<typename T>
class lua_type_registry_t
{
public:
    static void set_type_name(const string& str_)
    {
        type_name = str_;
    }
    static const char* get_type_name()
    {
        return type_name.c_str();
    }
    static bool is_registed()
    {
        return (true != type_name.empty());
    }

private:
    static string type_name;
};

template<typename T>
string lua_type_registry_t<T>::type_name;

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


typedef int (*index_impl_func_t)(lua_State*, void*, const char*);
typedef int (*newindex_impl_func_t)(lua_State*, void*, const char*, int);
struct lua_property_impl_t
{
    lua_property_impl_t():
        index_impl_func(NULL),
        newindex_impl_func(NULL),
        field_ptr(NULL)
    {}
    //! index impl function and newindex impl function
    index_impl_func_t    index_impl_func;
    newindex_impl_func_t newindex_impl_func;

    void* field_ptr;
};

template<typename PROPERTY_TYPE>
struct lua_property_udata_t : public lua_property_impl_t
{
    typedef property_registry_t<PROPERTY_TYPE> reg_property_t;
    reg_property_t                             reg_property;
};

typedef  pair<const char*, size_t> lua_string_t;
#endif
