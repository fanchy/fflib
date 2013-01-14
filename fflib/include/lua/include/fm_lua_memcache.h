#ifndef _FM_LUA_MEMCACHE_H_
#define _FM_LUA_MEMCACHE_H_

#include "fm_lua.h"
#include "memcache_client.h"

namespace fm_lua
{
    struct mget_result_t
    {
        size_t size()
        {
            return data.size();
        }
        string get(const string& key_)
        {
            return data[key_];
        }
        void begin()
        {
            it = data.begin();
        }
        void next()
        {
           ++ it;
        }
        bool is_end()
        {
            return it == data.end();
        }
        string fetch_key()
        {
            return it->first;
        }
        string fetch_value()
        {
            return it->second;
        }
        map<string, string> data;
        map<string, string>::iterator it;
    };

    static mget_result_t* exe_mget(memcache_client_t* client_, lua_table_t keys_)
    {
        mget_result_t* ret_value = new mget_result_t();

        vector<string> key_vt;

        for (keys_.begin(); false == keys_.end(); keys_.next())
        {
            string tmp_key, tmp_value;
            keys_.get_pair(tmp_key, tmp_value);
            key_vt.push_back(tmp_value);
        }
        keys_.release();

        client_->mget(key_vt, ret_value->data);
        return ret_value;
    }
};

LUA_REGISTER_BEGIN(fm_lua_memcache)

REGISTER_CLASS_BASE("mget_result_t", fm_lua::mget_result_t, void())
REGISTER_CLASS_METHOD("mget_result_t", "size", fm_lua::mget_result_t, &fm_lua::mget_result_t::size)
REGISTER_CLASS_METHOD("mget_result_t", "get", fm_lua::mget_result_t, &fm_lua::mget_result_t::get)
REGISTER_CLASS_METHOD("mget_result_t", "begin", fm_lua::mget_result_t, &fm_lua::mget_result_t::begin)
REGISTER_CLASS_METHOD("mget_result_t", "next", fm_lua::mget_result_t, &fm_lua::mget_result_t::next)
REGISTER_CLASS_METHOD("mget_result_t", "is_end", fm_lua::mget_result_t, &fm_lua::mget_result_t::is_end)
REGISTER_CLASS_METHOD("mget_result_t", "fetch_key", fm_lua::mget_result_t, &fm_lua::mget_result_t::fetch_key)
REGISTER_CLASS_METHOD("mget_result_t", "fetch_value", fm_lua::mget_result_t, &fm_lua::mget_result_t::fetch_value)
    
REGISTER_CLASS_BASE("memcache_client_t", memcache_client_t, void(const string&, int))
REGISTER_CLASS_METHOD("memcache_client_t", "set", memcache_client_t, &memcache_client_t::set)
REGISTER_CLASS_METHOD("memcache_client_t", "get", memcache_client_t, &memcache_client_t::get)
REGISTER_CLASS_METHOD("memcache_client_t", "del", memcache_client_t, &memcache_client_t::del)
REGISTER_CLASS_METHOD("memcache_client_t", "add", memcache_client_t, &memcache_client_t::add)
REGISTER_CLASS_METHOD("memcache_client_t", "increment", memcache_client_t, &memcache_client_t::increment)
REGISTER_CLASS_METHOD("memcache_client_t", "decrement", memcache_client_t, &memcache_client_t::decrement)

REGISTER_STATIC_FUNCTION("fm", "exe_mget", fm_lua::exe_mget)

LUA_REGISTER_END

#endif
