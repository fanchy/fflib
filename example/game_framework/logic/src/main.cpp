#include <iostream>
using namespace std;

#include "common_msg_def.h"
#include "rpc/ffrpc.h"
#include "log/log.h"

#include "logic_service.h"

#include "cqrs/bus_i.h"
#include "user/user_mgr.h"
#include "command/task_command.h"
#include "task/task_service.h"
#include "task/test_task.h"

using namespace ff;

int main(int argc, char* argv[])
{
    char buff[128];
    snprintf(buff, sizeof(buff), "tcp://%s:%s", "127.0.0.1", "10241");

    assert(0 == singleton_t<ffrpc_t>::instance().open(buff));

    logic_service_t logic_service;
    TASK_SERVICE.start();
    
    singleton_t<ffrpc_t>::instance().create_service_group("logic");
    singleton_t<ffrpc_t>::instance().create_service("logic", 0)
                                        .bind_service(&logic_service)
                                        .reg(&logic_service_t::login)
                                        .reg(&logic_service_t::logout)
                                        .reg(&logic_service_t::common_msg);


    signal_helper_t::wait();
    singleton_t<ffrpc_t>::instance().close();
    cout <<"\noh end\n";
    
    return 0;
}
