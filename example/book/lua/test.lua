
function foo(a)
    foo = foo_t:new()
    print("foooooo....:", foo, foo:get_pointer())
    foo:print()
    foo:delete()
    dumy()
end

