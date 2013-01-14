#include "lua_object.h"

lua_object_t::lua_object_t():
    m_ls(NULL),
    m_stack_index(-1),
    m_auto_pop_flag(false)
{
}

lua_object_t::lua_object_t(lua_State* ls_):
    m_ls(ls_),
    m_stack_index(-1),
    m_auto_pop_flag(false)
{
}

lua_object_t::~lua_object_t()
{
}

lua_object_t& lua_object_t::operator=(const lua_object_t& obj_)
{
    if (this == &obj_)
    {
        return *this;
    }

    release();

    init(obj_.get_lua_state(), obj_.stack_index(), obj_.get_flag());
    return *this;
}

void lua_object_t::init(lua_State* ls_, int nres_, bool auto_pop_flag_)
{
    m_ls   = ls_;
    m_stack_index = nres_;
    m_auto_pop_flag = auto_pop_flag_;
}

int lua_object_t::stack_index() const
{
    return m_stack_index;
}

void lua_object_t::release()
{
    if (NULL == m_ls)
    {
        return;
    }

    if (true == m_auto_pop_flag)
    {
        lua_remove(m_ls, m_stack_index);
        m_auto_pop_flag = false;
    }
}

lua_State* lua_object_t::get_lua_state() const
{
    return m_ls;
}

bool lua_object_t::get_flag() const
{
    return m_auto_pop_flag;
}

int  lua_object_t::type() const
{
    return lua_type(m_ls, m_stack_index);
}

string lua_object_t::type_name() const
{
    return lua_typename(m_ls, type());
}

bool lua_object_t::is_boolean() const
{
    return lua_isboolean(m_ls, m_stack_index);
}

bool lua_object_t::is_cfunction() const
{
    return lua_iscfunction(m_ls, m_stack_index);
}

bool lua_object_t::is_function() const
{
    return lua_isfunction(m_ls, m_stack_index);
}

bool lua_object_t::is_lightuserdata() const
{
    return lua_islightuserdata(m_ls, m_stack_index);
}

bool lua_object_t::is_nil() const
{
    return lua_isnil(m_ls, m_stack_index);
}

bool lua_object_t::is_none() const
{
    return lua_isnone(m_ls, m_stack_index);
}

bool lua_object_t::is_noneornil() const
{
    return lua_isnoneornil(m_ls, m_stack_index);
}

bool lua_object_t::is_number() const
{
    return lua_isnumber(m_ls, m_stack_index);
}

bool lua_object_t::is_string() const
{
    return lua_isstring(m_ls, m_stack_index);
}

bool lua_object_t::is_table() const
{
    return lua_istable(m_ls, m_stack_index);
}

bool lua_object_t::is_thread() const
{
    return lua_isthread(m_ls, m_stack_index);
}

bool lua_object_t::is_userdata() const
{
    return lua_isuserdata(m_ls, m_stack_index);
}

bool lua_object_t::to_boolean() const
{
    return lua_toboolean(m_ls, m_stack_index);
}

lua_function_t lua_object_t::to_cfunction() const
{
    return lua_tocfunction(m_ls, m_stack_index);
}

int lua_object_t::to_integer() const
{
    return lua_tointeger(m_ls, m_stack_index);
}

double lua_object_t::to_number() const
{
    return lua_tonumber(m_ls, m_stack_index);
}

const void* lua_object_t::to_pointer()
{
    return lua_topointer(m_ls, m_stack_index);
}

string lua_object_t::to_string() const
{
    return lua_tostring(m_ls, m_stack_index);
}

lua_State* lua_object_t::to_thread() const
{
    return lua_tothread(m_ls, m_stack_index);
}

void* lua_object_t::to_userdata() const
{
    return lua_touserdata(m_ls, m_stack_index);
}
