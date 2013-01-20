#include <iostream>
#include <string>
using namespace std;

#include "detail/task_queue_impl.h"
#include "thread.h"
#include "log.h"
#include "lua/fflua.h"

using namespace ff;

class base_t
{
public:
	void dump()
	{
		printf("in %s a:%d\n", __FUNCTION__, v);
	}
	int v;
};
class foo_t: public base_t
{
public:
	foo_t(int b)
	{
		printf("in %s b:%d this=%p\n", __FUNCTION__, b, this);
	}
	~foo_t()
	{
		printf("in %s\n", __FUNCTION__);
	}
	void print(int64_t a, base_t* p) const
	{
		printf("in foo_t::print a:%lld p:%p\n", a, p);
	}

	static void dumy()
	{
		printf("in %s\n", __FUNCTION__);
	}
	int a;
};

void dumy(map<string, string> ret)
{
	for (map<string, string>::iterator it =  ret.begin(); it != ret.end(); ++it)
	{
		printf("i:%s, val:%s:\n", it->first.c_str(), it->second.c_str());
	}
	printf("in %s\n", __FUNCTION__);
}

void lua_reg(lua_State* ls)
{
	fflua_register_t<base_t, ctor()>(ls, "base_t")
					.def(&base_t::dump, "dump")
					.def(&base_t::v, "v");


	fflua_register_t<foo_t, ctor(int)>(ls, "foo_t", "base_t")
				.def(&foo_t::print, "print")
				.def(&foo_t::a, "a");
	fflua_register_t<>(ls)
				.def(&dumy, "dumy");
}
int main(int argc, char* argv[])
{

	fflua_t fflua;
	fflua.load_file("test.lua");
	fflua.reg(lua_reg);
	vector<int> vt;vt.push_back(1);
	map<string, string> mp;mp["ok"]="nice";
	map<string, string> ret = fflua.call<map<string, string> >("foo", 1, vt, mp);

	for (map<string, string>::iterator it =  ret.begin(); it != ret.end(); ++it)
	{
		printf("i:%s, val:%s:\n", it->first.c_str(), it->second.c_str());
	}
    return 0;
}
