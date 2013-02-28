
#include "count/ffcount.h"
#include "rpc/broker_application.h"
#include "base/daemon_tool.h"
#include "base/arg_helper.h"

using namespace ff;
#include <stdio.h>

#define NUM 0
ffrpc_t* g_ffrpc = NULL;
int main(int argc, char* argv[])
{
    arg_helper_t arg_helper(argc, argv);
    if (false == arg_helper.is_enable_option("-l"))
    {
        printf("usage: app -l tcp://127.0.0.1:10241\n");
        return 1;
    }
    
    ffrpc_t ffrpc;
    g_ffrpc = &ffrpc;
    assert(0 == ffrpc.open(arg_helper.get_option_value("-l")) && "can't connnect to broker");
    
    assert(ffrpc.get_service("event_log_service", 0) && "event_log_service 0 not exist");
    
    event_log_t el("test"/*dbname*/,"dumy"/*tablename*/, "A,B,C"/*fields name*/);el.def(100, "p\"T'p", 5.4);
    ffrpc.async_call("event_log_service", 0, el);

    event_queryt_t::in_t in_msg;
    in_msg.db_name = "test";
    in_msg.sql = "select * from dumy limit 10";
    
    struct lambda_t
    {
        static void async_callback(event_queryt_t::out_t& msg_)
        {
            printf("=====>>>>> async_callback dump data [%s]<<<<<<=======\n", msg_.err_msg.c_str());
            ffdb_t::dump(msg_.ret_data, msg_.col_names);
        }
    };
    
    ffrpc.async_call("event_log_service", 0, in_msg, &lambda_t::async_callback);
    
    while (true)
    {
        event_log_t el("test", "dumy", "A,B,C");el.def(100, "p\"T'p", 5.4);
        g_ffrpc->async_call("event_log_service", 0, el);
        sleep(1);
        event_queryt_t::in_t in_msg;
        //in_msg.str_time = "2013/2";//! 查询1月的数据
        in_msg.db_name = "test";
        in_msg.sql = "select * from dumy order by autoid desc limit 5";
        event_queryt_t::out_t msg_;
        ffrpc.call("event_log_service", 0, in_msg, msg_);
        printf("=====>>>>> sync callback dump data [%s]<<<<<<=======\n", msg_.err_msg.c_str());
        ffdb_t::dump(msg_.ret_data, msg_.col_names);
    }
    signal_helper_t::wait();
    ffrpc.close();

    return 0;
}
