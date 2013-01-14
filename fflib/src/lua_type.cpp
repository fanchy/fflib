#include "lua_type.h"

lua_exception_t::lua_exception_t(const char* err_):
    m_err(err_)
{
}

lua_exception_t::lua_exception_t(const string& err_):
    m_err(err_)
{
}

const char* lua_exception_t::what() const throw ()
{
    return m_err.c_str();
}

lua_exception_t::~lua_exception_t() throw ()
{
}

void lua_helper::stack_dump(lua_State* ls_)
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

string lua_helper::dump_error(lua_State* ls_, const char *fmt, ...)
{
    string ret;
    char buff[1024];

    va_list argp;
    va_start(argp, fmt);
    vsnprintf(buff, sizeof(buff), fmt, argp);
    va_end(argp);

    ret = buff;
    snprintf(buff, sizeof(buff), " => lua tracback:%s", lua_tostring(ls_, -1));
    ret += buff;

    return ret;
}
