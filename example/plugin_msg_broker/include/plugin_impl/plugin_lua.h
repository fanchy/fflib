#ifndef _PLUGIN_LUA_H_
#define _PLUGIN_LUA_H_

#include <string>
#include <map>
using namespace std;
#include <lua.hpp>
#include "plugin_i.h"

using namespace ff;

class plugin_lua_t : public plugin_i
{
public:
    plugin_lua_t(const string& name_);
    ~plugin_lua_t();
    int start();
    int stop();

    int handle_broken(channel_ptr_t channel_);
    int handle_msg(const message_t& msg_, channel_ptr_t channel_);

    channel_ptr_t get_channel(long p);
private:
    int load_lua_mod();
    int call_lua_handle_msg(long val, const string& msg);
    int call_lua_handle_broken(long val);
private:
    string                                          m_lua_name;
    lua_State*                                      m_ls;
    map<long, channel_ptr_t>                        m_channel_mgr;
};
#endif
