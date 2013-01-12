#include "task_service.h"
#include "command/task_command.h"
#include "bus_i.h"

#include "user/user_mgr.h"
using namespace ff;

#include <iostream>
using namespace std;

task_service_t::task_service_t()
{
}

task_service_t::~task_service_t()
{
}

int task_service_t::start()
{
    subscriber_t subscriber;
    subscriber.reg<accept_task_cmd_t>(this)
              .reg<complete_task_cmd_t>(this);
    BUS.subscribe(subscriber);
    return 0;
}

int task_service_t::stop()
{
    return 0;
}

void task_service_t::handle(const accept_task_cmd_t& cmd_)
{
    cout <<"accept_task_cmd_t " << cmd_.tid << " xx\n";
    USER_MGR.get_user(cmd_.uid).get_tasks().accet_task(cmd_.tid);
}

void task_service_t::handle(const complete_task_cmd_t& cmd_)    
{
     USER_MGR.get_user(cmd_.uid).get_tasks().complete_task(cmd_.tid);
}
