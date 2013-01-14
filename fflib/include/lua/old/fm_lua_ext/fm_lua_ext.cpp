#include <iostream>
using namespace std;

#include <lua.hpp>
#include "fm_lua.h"
#include "fm_lua_json.h"
#include "fm_lua_memcache.h"
#include "fm_lua_dbmid_client.h"
#include "fm_lua_friend_client.h"

/*
static int l_dir (lua_State *L) {

    cout <<"xxxxxxxxxxxxx:l_dir\n";
    return 0;
}

static const struct luaL_reg mylib [] = {

    {"dir", l_dir},

    {NULL, NULL}

};

extern "C" {

int LUA_API luaopen_libfm_lua_ext (lua_State *L)
{
    cout <<"luaopen_libfm_lua_ext invoked\n";
    fm_lua_t lua(L);
    lua.multi_register(fm_lua_json);
    //luaL_openlib(L, "libfoo_ext", mylib, 0);
    return 0;
}

}
*/

DEFINE_LUA_EXT_BEGIN(libfm_lua_ext)
DEFINE_LUA_INTERFACE(fm_lua_json)
DEFINE_LUA_INTERFACE(fm_lua_memcache)
DEFINE_LUA_INTERFACE(fm_lua_dbmid_client)
DEFINE_LUA_INTERFACE(fm_lua_friend_client)
DEFINE_LUA_EXT_END

