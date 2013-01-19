#include <iostream>
#include <string>
using namespace std;

#include "detail/task_queue_impl.h"
#include "thread.h"
#include "log.h"
#include "lua/fflua.h"

using namespace ff;

class foo_t
{
public:
	~foo_t()
	{
		printf("in %s\n", __FUNCTION__);
	}
	void print() const
	{
		printf("in foo_t::print\n");
	}

	static void dumy()
	{
		printf("in %s\n", __FUNCTION__);
	}
	int a;
};

void dumy()
{
	printf("in %s\n", __FUNCTION__);
}

void lua_reg(lua_State* ls)
{
	fflua_register_t<foo_t>(ls, "foo_t")
				.def_class_func(&foo_t::print, "print")
				.def_class_property(&foo_t::a, "a")
				.def_func(&foo_t::dumy, "dumy");
}
int main(int argc, char* argv[])
{

	fflua_t fflua;
	fflua.load_file("test.lua");
	fflua.reg(lua_reg);
	fflua.call<bool>("foo", 1);


    return 0;
}
