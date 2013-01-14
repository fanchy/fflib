
mc_suite_t = {}
mc_suite_t.__index = mc_suite_t

function mc_suite_t:test_set()

    local host = "127.0.0.1"
    local port = 11211
    local key  = "hello"
    local val  = "world"

    local client = lua_memcache_client_t:new(host, port)

    -- timeout test code
    unit_assert(client:set(key, val, 1))
    unit_assert(val == client:get(key))
    os.execute("sleep 2")
    unit_assert("" == client:get(key))

    -- generate value code, for zlib
    local big_key  = "big_hello"
    local big_val = "a"
    for i = 1, 200
    do
        big_val = big_val .. "i"
    end

    for i = 1, 100
    do
        unit_assert(client:del(key))
        unit_assert("" == client:get(key))
    
        unit_assert(client:set(key, val, 0))
        unit_assert(val == client:get(key))
        
        unit_assert(client:del(big_key))
        unit_assert("" == client:get(big_key))
    
        unit_assert(client:set(big_key, big_val, 0))
        unit_assert(big_val == client:get(big_key))

    end
    client:delete() 
end

function mc_suite_t:test_get()

    local host = "127.0.0.1"
    local port = 11211
    local key  = "hello"
    local val  = "world"

    local client = lua_memcache_client_t:new(host, port)

    unit_assert(client:set(key, val, 0))
    for i = 1, 100
    do
        unit_assert(val == client:get(key))
    end

    client:delete() 
end

function mc_suite_t:test_del()

    local host = "127.0.0.1"
    local port = 11211
    local key  = "hello"
    local val  = "world"

    local client = lua_memcache_client_t:new(host, port)

    for i = 1, 100
    do
        unit_assert(client:set(key, val, 0))
        unit_assert(val == client:get(key))
        unit_assert(client:del(key))
        unit_assert("" == client:get(key))

    end

    client:delete() 
end

function mc_suite_t:test_add()

    local host = "127.0.0.1"
    local port = 11211
    local key  = "hello"
    local val  = "world"

    local client = lua_memcache_client_t:new(host, port)

    -- timeout test code
    unit_assert(client:del(key))
    unit_assert(client:add(key, val, 1))
    unit_assert(val == client:get(key))
    os.execute("sleep 2")
    unit_assert("" == client:get(key))

    -- generate value code, for zlib
    local big_key  = "big_hello"
    local big_val  = "a"
    for i = 1, 200
    do
        big_val = big_val .. "i"
    end

    for i = 1, 100
    do
        unit_assert(client:del(key))
        unit_assert("" == client:get(key))
        unit_assert(client:add(key, val, 0))
        unit_assert(val == client:get(key))
        
        unit_assert(client:set(key, val, 0))
        unit_assert(false == client:add(key, val, 0))

        unit_assert(val == client:get(key))

        unit_assert(client:del(big_key))
        unit_assert(client:add(big_key, big_val, 0))
        unit_assert(big_val == client:get(big_key))
    end

    client:delete() 
end

function mc_suite_t:test_increment()

    local host = "127.0.0.1"
    local port = 11211
    local key  = "num_test"
    local val  = "11223344"

    local client = lua_memcache_client_t:new(host, port)

    for i = 1, 100
    do
        unit_assert(client:del(key))
        unit_assert("" == client:get(key))

        unit_assert(client:set(key, val, 0))
        unit_assert(val == client:get(key))

        unit_assert(client:increment(key))
        unit_assert("11223345" == client:get(key))

    end

    client:delete() 
end

function mc_suite_t:test_decrement()

    local host = "127.0.0.1"
    local port = 11211
    local key  = "num_test"
    local val  = "11223344"

    local client = lua_memcache_client_t:new(host, port)

    for i = 1, 100
    do
        unit_assert(client:del(key))
        unit_assert("" == client:get(key))

        unit_assert(client:set(key, val, 0))
        unit_assert(val == client:get(key))

        unit_assert(client:decrement(key))
        unit_assert("11223343" == client:get(key))

    end

    client:delete()
end

function mc_suite_t:test_mget()

    local host = "127.0.0.1"
    local port = 11211
    local key_tb = {}
    local value_tb = {}

    local client = lua_memcache_client_t:new(host, port)

    for i = 1, 600
    do
        local tmp_k = "key_"..i
        local tmp_v = "value_"..i
        key_tb[i] = tmp_k
        value_tb[tmp_k] = tmp_v
        unit_assert(client:set(tmp_k, tmp_v, 0))
    end

    local num = 0
    for i = 1, 100
    do
        num = 0
        local ret = client:mget(key_tb)
        for k, v in pairs(value_tb)
        do
            unit_assert(ret[k] == v)
            if ret[k] == v
            then
                num = num + 1
            else
                print(k, v, ret[k])
            end
        end

    end
    --print (num, "xxx")

    client:delete()
end
