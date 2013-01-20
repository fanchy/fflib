
function foo(a)
    base = base_t:new()
    --base:dump()
    foo = foo_t:new(1234567)
    print("foooooo....:", foo, foo:get_pointer())
    foo:print(1, foo)
    --foo:delete()
    dumy()
    foo.v = 12345
    foo:dump()
    return foo
end

