#include "lua_table.h"
#include <iostream>
using namespace std;

lua_table_t::lua_table_t():
    m_ls(NULL),
    m_table_index(-1),
    m_is_end(true),
    m_auto_pop_flag(false)
{
}

lua_table_t::lua_table_t(lua_State* ls_, int nres_, bool auto_pop_flag_):
    m_ls(ls_),
    m_table_index(nres_),
    m_is_end(true),
    m_auto_pop_flag(auto_pop_flag_)
{
    if (ls_)
    {
        init(ls_, nres_, auto_pop_flag_);
    }
}

lua_table_t::~lua_table_t()
{
}

void lua_table_t::init(lua_State* ls_, int nres_, bool auto_pop_flag_)
{
    m_ls   = ls_;
    m_table_index = nres_;
    m_auto_pop_flag = auto_pop_flag_;

}

bool lua_table_t::end()
{
    return m_is_end;
}

bool lua_table_t::next()
{
    if (true == m_is_end)
    {
        return m_is_end;
    }

    lua_pop(m_ls, 1);
    if (lua_next(m_ls, m_table_index) == 0)
    {
        m_is_end = true;
    }

    return m_is_end;
}

void lua_table_t::begin()
{
    if (NULL != m_ls)
    {
        if (false == m_is_end)
        {
            //! pop the key and value
            lua_pop(m_ls, 2);
            m_is_end = true;
        }

        locat_begin_i();
    }
}

int  lua_table_t::table_index() const
{
    return m_table_index;
}

void lua_table_t::release()
{
    if (NULL == m_ls)
    {
        return;
    }

    if (false == m_is_end)
    {
        //! pop the key and value
        lua_pop(m_ls, 2);
        m_is_end = true;
    }

    if (true == m_auto_pop_flag)
    {
        lua_remove(m_ls, m_table_index);
        m_auto_pop_flag = false;
    }
}

void lua_table_t::locat_begin_i()
{
    lua_pushnil(m_ls);
    if (lua_next(m_ls, m_table_index) == 0)
    {
        m_is_end = true;
    }
    else
    {
        m_is_end = false;
    }
}

int lua_table_t::get_value(lua_table_t& param_)
{
    if (!lua_istable(m_ls, -1))
    {
        return -1;
    }

    param_.init(m_ls, lua_gettop(m_ls), false);//! auto relese
    return 0;
}
auto_lua_table_t::auto_lua_table_t()
{
}

auto_lua_table_t::~auto_lua_table_t()
{
    _super_t::release();
}
