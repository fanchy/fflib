
#ifndef _FF_TASK_COMMAND_H_
#define _FF_TASK_COMMAND_H_

#include "command_i.h"

namespace ff
{

class accept_task_cmd_t: public user_command_t<accept_task_cmd_t>
{
public:
    virtual void enc()
    {
        encoder() << tid;
    }
    virtual void dec()
    {
        decoder() >> tid;
    }

    uint32_t tid;
};

class complete_task_cmd_t: public user_command_t<complete_task_cmd_t>
{
public:
    virtual void enc()
    {
        encoder() << tid;
    }
    virtual void dec()
    {
        decoder() >> tid;
    }

    uint32_t tid;
};

}
#endif
