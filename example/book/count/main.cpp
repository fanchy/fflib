
#include "count/ffcount.h"
#include "rpc/broker_application.h"
#include "base/daemon_tool.h"
#include "base/arg_helper.h"

using namespace ff;
#include <stdio.h>

int main(int argc, char* argv[])
{
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
    
    signal_helper_t::wait();
    singleton_t<msg_bus_t>::instance().close();
    ffcount_service.stop();
    return 0;
}
