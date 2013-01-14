#ifndef _FM_LUA_FRIEND_CLIENT_H_
#define _FM_LUA_FRIEND_CLIENT_H_

#include "fm_lua.h"
#include "friend_client.h"

LUA_REGISTER_BEGIN(fm_lua_friend_client)

REGISTER_CLASS_BASE("friend_client_t", friend_client_t, void(const string&, int))
REGISTER_CLASS_METHOD("friend_client_t", "login", friend_client_t, &friend_client_t::login)

//! --------------------- use platform uid -------------------------

REGISTER_CLASS_METHOD("friend_client_t", "get_version", friend_client_t, &friend_client_t::get_version)
REGISTER_CLASS_METHOD("friend_client_t", "get_self_info", friend_client_t, &friend_client_t::get_self_info)
REGISTER_CLASS_METHOD("friend_client_t", "get_app_ids", friend_client_t, &friend_client_t::get_app_ids)
REGISTER_CLASS_METHOD("friend_client_t", "get_all_ids", friend_client_t, &friend_client_t::get_all_ids)
REGISTER_CLASS_METHOD("friend_client_t", "get_all_fds", friend_client_t, &friend_client_t::get_all_fds)

//! --------------------- use fm uid -------------------------
REGISTER_CLASS_METHOD("friend_client_t", "fm_get_version", friend_client_t, &friend_client_t::fm_get_version)
REGISTER_CLASS_METHOD("friend_client_t", "fm_get_self_info", friend_client_t, &friend_client_t::fm_get_self_info)
REGISTER_CLASS_METHOD("friend_client_t", "fm_get_app_ids", friend_client_t, &friend_client_t::fm_get_app_ids)
REGISTER_CLASS_METHOD("friend_client_t", "fm_get_all_ids", friend_client_t, &friend_client_t::fm_get_all_ids)
REGISTER_CLASS_METHOD("friend_client_t", "fm_get_all_fds", friend_client_t, &friend_client_t::fm_get_all_fds)

REGISTER_CLASS_METHOD("friend_client_t", "check_app_fans", friend_client_t, &friend_client_t::check_app_fans)
REGISTER_CLASS_METHOD("friend_client_t", "check_all_friend", friend_client_t, &friend_client_t::check_all_friend)
REGISTER_CLASS_METHOD("friend_client_t", "check_app_friend", friend_client_t, &friend_client_t::check_app_friend)
REGISTER_CLASS_METHOD("friend_client_t", "service_info", friend_client_t, &friend_client_t::service_info)\

LUA_REGISTER_END

#endif
