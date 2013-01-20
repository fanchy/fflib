#include <iostream>
using namespace std;

#include "common_msg_def.h"
#include "rpc/msg_bus.h"
#include "log/log.h"

#include "gateway_service.h"
#include "rpc/broker_application.h"

using namespace ff;

/*
 login_t::in_t in;
 in.uid = 100;
 in.set_gate();
 in.passwd = "Xxvefds";
 
 string data = in.encode();
 gate_msg_tool_t msg_tool;
 msg_tool.gate_decode(data.substr(8));
 
 login_t::in_t foo;
 foo.decode(msg_tool.encode().substr(8));
 
 cout << msg_tool.get_name() <<" " << foo.uid  << " " << foo.passwd <<"\n";
 return 0;
 */
int main(int argc, char* argv[])
{
    broker_application_t::run(argc, argv);

    char buff[128];
    snprintf(buff, sizeof(buff), "tcp://%s:%s", "127.0.0.1", "10241");

    assert(0 == singleton_t<msg_bus_t>::instance().open(buff));

    gateway_service_t gateway_service;
    singleton_t<msg_bus_t>::instance().create_service_group("gateway");
    singleton_t<msg_bus_t>::instance().create_service("gateway", 0)
                                        .bind_service(&gateway_service)
                                        .reg(&gateway_service_t::unicast)
                                        .reg(&gateway_service_t::broadcast);


    net_factory_t::gateway_listen("-gateway_listen tcp://127.0.0.1:20241", &gateway_service);

    signal_helper_t::wait();
    singleton_t<msg_bus_t>::instance().close();
    cout <<"\noh end\n";
    
    return 0;
}
