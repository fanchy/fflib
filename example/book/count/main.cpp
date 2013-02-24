
#include "count/ffcount.h"
#include "rpc/broker_application.h"
#include "base/daemon_tool.h"
#include "base/arg_helper.h"

using namespace ff;
#include <stdio.h>

int main(int argc, char* argv[])
{
    if (argc == 1)
    {
        printf("usage: app -l tcp://127.0.0.1:10241 -http tcp://127.0.0.1:8080\n");
        return 1;
    }
    arg_helper_t arg_helper(argc, argv);
    broker_application_t::run(argc, argv);

    assert(0 == singleton_t<msg_bus_t>::instance().open(arg_helper.get_option_value("-l")) && "can't connnect to broker");
    if (arg_helper.is_enable_option("-d"))
    {
        daemon_tool_t::daemon();
    }
    
    ffcount_service_t ffcount_service;
    ffcount_service.start();
    
    singleton_t<msg_bus_t>::instance().create_service_group("event_log_service");
    singleton_t<msg_bus_t>::instance().create_service("event_log_service", 0)
            .bind_service(&ffcount_service)
            .reg(&ffcount_service_t::save_event)
            .reg(&ffcount_service_t::query);
    
    acceptor_i* p_http = NULL;
    if (arg_helper.is_enable_option("-http"))
    {
        p_http = net_factory_t::http_listen(arg_helper.get_option_value("-http"), &ffcount_service);
    }
    
    signal_helper_t::wait();
    singleton_t<msg_bus_t>::instance().close();
    ffcount_service.stop();
    return 0;
}
