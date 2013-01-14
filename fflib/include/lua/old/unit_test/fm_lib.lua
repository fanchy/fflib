
package.cpath = package.cpath.. ";../fm_lua_ext/.libs/?.so"
print (package.cpath)
if fm_lua_t == nil
then
    require ("libfm_lua_ext")
end
