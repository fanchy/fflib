#ifndef _FM_LUA_TASK_H_
#define _FM_LUA_TASK_H_

#include "task_client.h"

LUA_REGISTER_BEGIN(fm_lua_task_client)

REGISTER_CLASS_BASE("action_count_t", fm_task_client::msg_action_item_t, void())
REGISTER_CLASS_PROPERTY(fm_task_client::msg_action_item_t, "cmd", &fm_task_client::msg_action_item_t::cmd)
REGISTER_CLASS_PROPERTY(fm_task_client::msg_action_item_t, "action", &fm_task_client::msg_action_item_t::action)
REGISTER_CLASS_PROPERTY(fm_task_client::msg_action_item_t, "object", &fm_task_client::msg_action_item_t::object)
REGISTER_CLASS_PROPERTY(fm_task_client::msg_action_item_t, "value", &fm_task_client::msg_action_item_t::value)
REGISTER_CLASS_PROPERTY(fm_task_client::msg_action_item_t, "scene_id", &fm_task_client::msg_action_item_t::scene_id)

REGISTER_CLASS_BASE("msg_triggle_cond_item_t", task_client_t::msg_triggle_cond_item_t, void())
REGISTER_CLASS_PROPERTY(task_client_t::msg_triggle_cond_item_t, "type", &task_client_t::msg_triggle_cond_item_t::type)
REGISTER_CLASS_PROPERTY(task_client_t::msg_triggle_cond_item_t, "object", &task_client_t::msg_triggle_cond_item_t::object)
REGISTER_CLASS_PROPERTY(task_client_t::msg_triggle_cond_item_t, "value", &task_client_t::msg_triggle_cond_item_t::value)
REGISTER_CLASS_METHOD("msg_triggle_cond_item_t", "set", task_client_t::msg_triggle_cond_item_t, &task_client_t::msg_triggle_cond_item_t::set)

REGISTER_CLASS_BASE("action_item_array_t", task_client_t::action_item_array_t, void())
REGISTER_CLASS_METHOD("action_item_array_t", "size", task_client_t::action_item_array_t, &task_client_t::action_item_array_t::size)
REGISTER_CLASS_METHOD("action_item_array_t", "at", task_client_t::action_item_array_t, &task_client_t::action_item_array_t::at)
REGISTER_CLASS_METHOD("action_item_array_t", "clear", task_client_t::action_item_array_t, &task_client_t::action_item_array_t::clear)
REGISTER_CLASS_METHOD("action_item_array_t", "push_back", task_client_t::action_item_array_t, &task_client_t::action_item_array_t::push_back_ptr)

REGISTER_CLASS_BASE("triggle_item_array_t", task_client_t::triggle_item_array_t, void())
REGISTER_CLASS_METHOD("triggle_item_array_t", "size", task_client_t::triggle_item_array_t, &task_client_t::triggle_item_array_t::size)
REGISTER_CLASS_METHOD("triggle_item_array_t", "at", task_client_t::triggle_item_array_t, &task_client_t::triggle_item_array_t::at)
REGISTER_CLASS_METHOD("triggle_item_array_t", "clear", task_client_t::triggle_item_array_t, &task_client_t::triggle_item_array_t::clear)
REGISTER_CLASS_METHOD("triggle_item_array_t", "push_back", task_client_t::triggle_item_array_t, &task_client_t::triggle_item_array_t::push_back_ptr)

REGISTER_CLASS_BASE("task_client_t", task_client_t, void(const string& host_, int port_))
REGISTER_CLASS_METHOD("task_client_t", "login", task_client_t, &task_client_t::login)
REGISTER_CLASS_METHOD("task_client_t", "get_tasklist", task_client_t, &task_client_t::get_tasklist)
REGISTER_CLASS_METHOD("task_client_t", "get_tasklist_and_trigger", task_client_t, &task_client_t::get_tasklist_and_trigger)
REGISTER_CLASS_METHOD("task_client_t", "accept_task", task_client_t, &task_client_t::accept_task)
REGISTER_CLASS_METHOD("task_client_t", "reject_task", task_client_t, &task_client_t::reject_task)
REGISTER_CLASS_METHOD("task_client_t", "accept_task_award", task_client_t, &task_client_t::accept_task_award)
REGISTER_CLASS_METHOD("task_client_t", "action_increment", task_client_t, &task_client_t::action_increment)
REGISTER_CLASS_METHOD("task_client_t", "action_set", task_client_t, &task_client_t::action_set)
REGISTER_CLASS_METHOD("task_client_t", "mrequest", task_client_t, &task_client_t::action_mrequest)
REGISTER_CLASS_METHOD("task_client_t", "trigger_task", task_client_t, &task_client_t::trigger_task)

REGISTER_CLASS_METHOD("task_client_t", "get_task_define", task_client_t, &task_client_t::get_task_define)
REGISTER_CLASS_METHOD("task_client_t", "jump2task", task_client_t, &task_client_t::jump2task)

REGISTER_CLASS_METHOD("task_client_t", "update_task_define", task_client_t, &task_client_t::update_task_define)
REGISTER_CLASS_METHOD("task_client_t", "del_user", task_client_t, &task_client_t::del_user)

LUA_REGISTER_END

#endif
