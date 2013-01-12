
#ifndef _FF_TASK_SERVICE_H_
#define _FF_TASK_SERVICE_H_

#include "utility/singleton.h"

namespace ff
{
class accept_task_cmd_t;
class complete_task_cmd_t;

class task_service_t
{
public:
    task_service_t();
    ~task_service_t();

    int start();
    int stop();
    
    void handle(const accept_task_cmd_t& cmd_);
    void handle(const complete_task_cmd_t& cmd_);
};

#define TASK_SERVICE singleton_t<task_service_t >::instance()
}

#endif
