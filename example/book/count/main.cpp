
#include "count/ffcount.h"
#include "rpc/broker_application.h"

using namespace ff;
#include <stdio.h>

int main(int argc, char* argv[])
{
    broker_application_t::run(argc, argv);
    
    char buff[128];
    snprintf(buff, sizeof(buff), "tcp://%s:%s", "127.0.0.1", "10241");

    assert(0 == singleton_t<msg_bus_t>::instance().open(buff) && "can't connnect to broker");
    
    ffcount_service_t ffcount_service;
    ffcount_service.start();
    
    
    singleton_t<msg_bus_t>::instance().create_service_group("event_log_service");
    singleton_t<msg_bus_t>::instance().create_service("event_log_service", 0)
            .bind_service(&ffcount_service)
            .reg(&ffcount_service_t::save_event);
    
    
    assert(singleton_t<msg_bus_t>::instance().get_service_group("event_log_service") && "event_log_service group not exist");

    assert(singleton_t<msg_bus_t>::instance().get_service_group("event_log_service")->get_service(0) && "event_log_service 0 not exist");
    
    event_log_t el("dumy", "A,B,C");el.def(100, "pp", 5.4);
     struct lambda_t
    {
        static void callback(event_ret_t& msg_)
        {
            
        }
     };
     singleton_t<msg_bus_t>::instance().get_service_group("event_log_service")->get_service(0)->async_call(el);
    //ffcount_t ffcount;
    
    //int ret = ffcount.save_event(el);
    //printf("add event ret=%d, safe=%d\n", ret,  sqlite3_threadsafe());
    //ffcount.query("ALTER TABLE dumy ADD qq varchar(255) default 'TT'");
    //ffcount.query("select * from dumy");
    
    //ffcount_service.save_event(el);
    signal_helper_t::wait();
    singleton_t<msg_bus_t>::instance().close();
    ffcount_service.stop();
    return 0;
}
