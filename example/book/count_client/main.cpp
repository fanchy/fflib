
#include "count/ffcount.h"
#include "rpc/broker_application.h"
#include "base/daemon_tool.h"
#include "base/arg_helper.h"

using namespace ff;
#include <stdio.h>

#define NUM 0
int main(int argc, char* argv[])
{
    arg_helper_t arg_helper(argc, argv);
    if (false == arg_helper.is_enable_option("-l"))
    {
        printf("usage: app -l tcp://127.0.0.1:10241\n");
        return 1;
    }
    
    assert(0 == singleton_t<msg_bus_t>::instance().open(arg_helper.get_option_value("-l")) && "can't connnect to broker");
    
    assert(singleton_t<msg_bus_t>::instance().get_service_group("event_log_service") && "event_log_service group not exist");

    assert(singleton_t<msg_bus_t>::instance().get_service_group("event_log_service")->get_service(0) && "event_log_service 0 not exist");
    
    event_log_t el("dumy", "A,B,C");el.def(100, "p\"T'p", 5.4);
    singleton_t<msg_bus_t>::instance().get_service_group("event_log_service")->get_service(0)->async_call(el);
    for (int i = 0; i < NUM; ++i)
    {
        char buff[64];
        snprintf(buff, sizeof(buff), "dumy_%d", i%8);
        event_log_t el(buff, "A,B,C");el.def(100, "pp", 5.4);
        singleton_t<msg_bus_t>::instance().get_service_group("event_log_service")->get_service(0)->async_call(el);
    }
    
    event_queryt_t::in_t in_msg;
    in_msg.table_name = "dumy";
    in_msg.sql = "select * from dumy";
    
    struct lambda_t
    {
        static void callback(event_queryt_t::out_t& msg_)
        {
            printf("=====>>>>> callback dump data [%s]<<<<<<=======\n", msg_.err_msg.c_str());
            ffdb_t::dump(msg_.ret_data, msg_.col_names);
            
            event_log_t el("dumy", "A,B,C");el.def(100, "p\"T'p", 5.4);
            singleton_t<msg_bus_t>::instance().get_service_group("event_log_service")->get_service(0)->async_call(el);
            for (int i = 0; i < NUM; ++i)
            {
                char buff[64];
                snprintf(buff, sizeof(buff), "dumy_%d", i%8);
                event_log_t el(buff, "A,B,C");el.def(100, "pp", 5.4);
                singleton_t<msg_bus_t>::instance().get_service_group("event_log_service")->get_service(0)->async_call(el);
            }
            sleep(1);
            
            event_queryt_t::in_t in_msg;
            //in_msg.str_time = "2013/2";//! 查询1月的数据
            in_msg.table_name = "dumy";
            in_msg.sql = "select * from dumy order by logtime desc limit 5";
            singleton_t<msg_bus_t>::instance().get_service_group("event_log_service")->get_service(0)->async_call(in_msg, &lambda_t::callback);
        }
    };
    
    singleton_t<msg_bus_t>::instance().get_service_group("event_log_service")->get_service(0)->async_call(in_msg, &lambda_t::callback);
    signal_helper_t::wait();
    singleton_t<msg_bus_t>::instance().close();

    return 0;
}
