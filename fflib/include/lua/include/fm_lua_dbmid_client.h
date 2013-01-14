#ifndef _FM_LUA_DBMID_CLIENT_H_
#define _FM_LUA_DBMID_CLIENT_H_

#include "fm_lua.h"
#include "dbmid_client.h"

LUA_REGISTER_BEGIN(fm_lua_dbmid_client)

REGISTER_CLASS_BASE("dbmid_client_t", dbmid_client_t, void(const string&, int))
REGISTER_CLASS_METHOD("dbmid_client_t", "add", dbmid_client_t, &dbmid_client_t::add)
REGISTER_CLASS_METHOD("dbmid_client_t", "update", dbmid_client_t, &dbmid_client_t::update)
REGISTER_CLASS_METHOD("dbmid_client_t", "del", dbmid_client_t, &dbmid_client_t::del)
REGISTER_CLASS_METHOD("dbmid_client_t", "find", dbmid_client_t, &dbmid_client_t::find)
REGISTER_CLASS_METHOD("dbmid_client_t", "find_all", dbmid_client_t, &dbmid_client_t::find_all)
REGISTER_CLASS_METHOD("dbmid_client_t", "find_all_limit", dbmid_client_t, &dbmid_client_t::find_all_limit)

LUA_REGISTER_END

#endif
