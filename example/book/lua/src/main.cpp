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
void print()
{}
};

LUA_REGISTER_BEGIN(ext)
REGISTER_CLASS_BASE("foo_t", foo_t, void())
REGISTER_CLASS_METHOD("foo_t", "print", foo_t, &foo_t::print)
LUA_REGISTER_END

int main(int argc, char* argv[])
{

	fflua_t fflua;
	fflua.multi_register(ext);
	fflua.load_file("test.lua");
	fflua.call<bool>("foo", 1);

    return 0;
}
