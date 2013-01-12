
#ifndef _USER_TASKS_I_
#define _USER_TASKS_I_

#include "aggregate_root_i.h"
#include "bus_i.h"

#include <map>
#include <stdexcept>
using namespace std;

namespace ff
{

class task_accepted_t;
class task_completed_t;
class task_modified_t;

class user_tasks_t: public aggregate_root_i
{
public:
    typedef runtime_error task_exception_t;
    enum task_status_e
    {
        TASK_UNACCEPTED,
        TASK_ACCEPTED,
        TASK_COMPLETED,
    };
    struct task_ino_t
    {
        task_ino_t(int tid = 0, int dest_value_ = 0, int status_ = TASK_UNACCEPTED):
            task_id(tid),
            status(status_),
            current_value(0),
            dest_value(dest_value_)
        {}
        
        int task_id;
        int status;
        int current_value;
        int dest_value;
    };
    typedef map<int, task_ino_t> task_set_t;
public:
    user_tasks_t();
    ~user_tasks_t();

    void accet_task(int task_id_);
    void complete_task(int task_id_);
    void modify_task(int task_id_, int value_);
    
    template <typename T>
    void apply_change(const T& event_, bool new_change_ = true)
    {
        apply(event_);
        if (new_change_)
        {
            BUS.publish(event_);
        }
    }

    void apply(const task_accepted_t& event_);
    void apply(const task_completed_t& event_);
    void apply(const task_modified_t& event_);
    
protected:
    task_set_t  m_tasks;

};

}
#endif
