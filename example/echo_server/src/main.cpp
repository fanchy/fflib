#include <iostream>
using namespace std;

#include "msg_def.h"
#include "rpc/msg_bus.h"
#include "log/log.h"

using namespace ff;

struct echo_service_t
{
public:
    void echo(echo_t::in_t& in_msg_, rpc_callcack_t<echo_t::out_t>& cb_)
    {
        logtrace((FF, "echo_service_t::echo done value<%s>", in_msg_.value.c_str()));
        echo_t::out_t out;
        out.value = in_msg_.value;
        cb_(out);
    }
};

rpc_service_t* g_rpc_service = NULL;
int main(int argc, char* argv[])
{
    int g_index = 1;
    if (argc > 1)
    {
        g_index = atoi(argv[1]);
    }
    char buff[128];
    snprintf(buff, sizeof(buff), "tcp://%s:%s", "127.0.0.1", "10241");
    
    msg_bus_t msg_bus;
    assert(0 == singleton_t<msg_bus_t>::instance().open("tcp://127.0.0.1:10241") && "can't connnect to broker");

    echo_service_t f;

    singleton_t<msg_bus_t>::instance().create_service_group("echo");
    singleton_t<msg_bus_t>::instance().create_service("echo", g_index)
            .bind_service(&f)
            .reg(&echo_service_t::echo);
    
    signal_helper_t::wait();

    singleton_t<msg_bus_t>::instance().close();
    //usleep(1000);
    cout <<"\noh end\n";
    return 0;
}
