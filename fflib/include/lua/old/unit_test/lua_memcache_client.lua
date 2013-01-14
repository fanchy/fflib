require ("fm_lib")

lua_memcache_client_t = {}
lua_memcache_client_t.__index = function(t_, k_)
    local f = rawget(lua_memcache_client_t, k_)
    if f then return f end

    return function(tb_, arg1_, arg2_, arg3_, arg4, arg5, arg6)
            return (tb_.client)[k_](tb_.client, arg1_, arg2_, arg3_, arg4, arg5, arg6)
    end
end

function lua_memcache_client_t:new(host_, port_)

    local c = memcache_client_t:new(host_, port_)
    local ret = getmetatable(c)
    ret.__gc = function(ud_)
        if 0 ~= ud_:get_pointer()
        then
            ud_:delete()
        end
    end

    local ret = 
    {
        ["client"] = c,
    }
    setmetatable(ret, self)
    return ret
end

function lua_memcache_client_t:mget(key_tb_)

    local ret = {}
    local mget_result = fm:exe_mget(self.client, key_tb_)

    mget_result:begin()
    while false == mget_result:is_end()
    do
        ret[mget_result:fetch_key()] = mget_result:fetch_value()
        mget_result:next()
    end
    mget_result:delete()

    return ret
end

function lua_memcache_client_t:delete()
    self.client:delete()
    self = nil
end
