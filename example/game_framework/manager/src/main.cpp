#include <iostream>
using namespace std;

#include "common_msg_def.h"
#include "rpc/ffrpc.h"
#include "log/log.h"

#include "manager_service.h"

using namespace ff;

int main(int argc, char* argv[])
{
    char buff[128];
    snprintf(buff, sizeof(buff), "tcp://%s:%s", "127.0.0.1", "10241");

    assert(0 == singleton_t<ffrpc_t>::instance().open(buff));

    manager_service_t manager_service;
    singleton_t<ffrpc_t>::instance().create_service_group("manager");
    singleton_t<ffrpc_t>::instance().create_service("manager", 0)
                                        .bind_service(&manager_service)
                                        .reg(&manager_service_t::login)
                                        .reg(&manager_service_t::logout);


    signal_helper_t::wait();
    singleton_t<ffrpc_t>::instance().close();
    cout <<"\noh end\n";
    
    return 0;
}
