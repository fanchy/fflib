
function foo(a, vt, tb)
    print(tb)
    table.insert(tb, 2)
    table.insert(tb, 3)
    for k, v in pairs(tb)
    do
        print(k, v)
    end
    
    base = base_t:new()
    --base:dump()
    foo = foo_t:new(1234567)
    print("foooooo....:", foo, foo:get_pointer())
    foo:print(1, foo)
    --foo:delete()
    dumy(tb)
    foo.v = 12345
    foo:dump()
    return tb
end

