require ("lua_memcache_client")
require ("memcache_test_suite")

unit_test_error_info = {}
unit_test_done_count = 0
assert_done_count    = 0

function unit_assert(b)
    if false == b
    then
        local file = debug.getinfo(2).short_src
        local func = debug.traceback()
        local info = debug.getinfo(4)
        local line = debug.getinfo(2).currentline
        local val  = string.format("%s:%s line:%s", file, info.name or "" , line)
        unit_test_error_info[val] = "assert failed"
    end
    assert_done_count = assert_done_count + 1
end

function exe_unit_test() 
for i = 1, 1 do

    for suite_k, suite_v in pairs(_G)
    do
        -- print(suite_k, suite_v)
        if nil ~= suite_k:find("suite_t")
        then 
            for func_name, func in pairs(suite_v)
            do
                if func_name ~= "__index" and "function" == type(func)
                then
                    local tmp = unit_test_error_info
                    unit_test_error_info = {}
                    print (string.format("test_case:%s", func_name), func, "executing...")       
                    func(suite_v)
                    print (string.format("test_case:%s", func_name), func, "done.")
                    for err, info in pairs(unit_test_error_info)
                    do
                        tmp[func_name.." in "..err] = info
                    end
                    unit_test_error_info = tmp
                    unit_test_done_count = unit_test_done_count + 1
                end
            end
        end
    end

end
    local err_num  = 0
    local err_info = ""

    for k ,v in pairs(unit_test_error_info)
    do
        -- print(k, v)
        err_info = err_info .. k .. "   " .. v.."\n"
        err_num = err_num + 1
    end
    print (string.format("\n--------------------------------------------- test_case_done[%s], error[%s], assert_done[%s]",
                         unit_test_done_count, err_num, assert_done_count))
    print(err_info)
    
    print (string.format("--------------------------------------------------------------------------------------------"))
end

exe_unit_test()
