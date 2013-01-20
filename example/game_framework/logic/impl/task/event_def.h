
#ifndef _EVENT_DEF_I_
#define _EVENT_DEF_I_

#include "cqrs/event_i.h"

namespace ff
{

class task_accepted_t: public event_t<task_accepted_t>
{
public:
    task_accepted_t(int task_id_ =0, int dest_value_ = 0):
        task_id(task_id_),
        dest_value(dest_value_)
    {}
    int task_id;
    int dest_value;
};

class task_completed_t: public event_t<task_completed_t>
{
public:
    task_completed_t(int tid_):
        task_id(tid_)
    {}
    int task_id;
};
    
class task_modified_t: public event_t<task_modified_t>
{
public:
    task_modified_t(int tid_, int value_):
        task_id(tid_),
        value(value_)
    {}
    int task_id;
    int value;
};

}
#endif
