#ifndef _FM_LUA_ACCUMULATE_H_
#define _FM_LUA_ACCUMULATE_H_

#include "accumulate_client.h"

LUA_REGISTER_BEGIN(fm_lua_accumulate_client)

REGISTER_CLASS_BASE("multi_attr_t", fm_accumulate_client::multi_attr_t, void())
REGISTER_CLASS_PROPERTY(fm_accumulate_client::multi_attr_t, "attr", &fm_accumulate_client::multi_attr_t::attr)
REGISTER_CLASS_PROPERTY(fm_accumulate_client::multi_attr_t, "attr_value", &fm_accumulate_client::multi_attr_t::attr_value)
REGISTER_CLASS_PROPERTY(fm_accumulate_client::multi_attr_t, "count", &fm_accumulate_client::multi_attr_t::count)
REGISTER_CLASS_METHOD("multi_attr_t", "set", fm_accumulate_client::multi_attr_t, &fm_accumulate_client::multi_attr_t::set)

REGISTER_CLASS_BASE("multi_attr_array_t", accumulate_client_t::multi_attr_array_t, void())
REGISTER_CLASS_METHOD("multi_attr_array_t", "size", accumulate_client_t::multi_attr_array_t, &accumulate_client_t::multi_attr_array_t::size)
REGISTER_CLASS_METHOD("multi_attr_array_t", "at", accumulate_client_t::multi_attr_array_t, &accumulate_client_t::multi_attr_array_t::at)
REGISTER_CLASS_METHOD("multi_attr_array_t", "clear", accumulate_client_t::multi_attr_array_t, &accumulate_client_t::multi_attr_array_t::clear)
REGISTER_CLASS_METHOD("multi_attr_array_t", "push_back", accumulate_client_t::multi_attr_array_t, &accumulate_client_t::multi_attr_array_t::push_back_ptr)

REGISTER_CLASS_BASE("accumulate_client_t", accumulate_client_t, void(const string& host_, int port_))
REGISTER_CLASS_METHOD("accumulate_client_t", "action", accumulate_client_t, &accumulate_client_t::action)
REGISTER_CLASS_METHOD("accumulate_client_t", "user", accumulate_client_t, &accumulate_client_t::user)
REGISTER_CLASS_METHOD("accumulate_client_t", "tag_set", accumulate_client_t, &accumulate_client_t::tag_set)
REGISTER_CLASS_METHOD("accumulate_client_t", "init_user_key", accumulate_client_t, &accumulate_client_t::init_user_key)
REGISTER_CLASS_METHOD("accumulate_client_t", "update_user_key", accumulate_client_t, &accumulate_client_t::update_user_key)
REGISTER_CLASS_METHOD("accumulate_client_t", "action_multi_attr", accumulate_client_t, &accumulate_client_t::action_multi_attr)
REGISTER_CLASS_METHOD("accumulate_client_t", "set_tag_time", accumulate_client_t, &accumulate_client_t::set_tag_time)
REGISTER_CLASS_METHOD("accumulate_client_t", "tag_set_data", accumulate_client_t, &accumulate_client_t::tag_set_data)
REGISTER_CLASS_METHOD("accumulate_client_t", "login", accumulate_client_t, &accumulate_client_t::login)
REGISTER_CLASS_METHOD("accumulate_client_t", "set_user_data", accumulate_client_t, &accumulate_client_t::set_user_data)

REGISTER_CLASS_METHOD("accumulate_client_t", "set_user_data_rb", accumulate_client_t, &accumulate_client_t::set_user_data_rb)
REGISTER_CLASS_METHOD("accumulate_client_t", "set_user_task", accumulate_client_t, &accumulate_client_t::set_user_task)

REGISTER_CLASS_METHOD("accumulate_client_t", "set_user_build", accumulate_client_t, &accumulate_client_t::set_user_build)
REGISTER_CLASS_METHOD("accumulate_client_t", "request_accumulate", accumulate_client_t, &accumulate_client_t::request_accumulate)
REGISTER_CLASS_METHOD("accumulate_client_t", "get_black_list", accumulate_client_t, &accumulate_client_t::get_black_list)

LUA_REGISTER_END

#endif
