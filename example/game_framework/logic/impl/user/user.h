
#ifndef _FF_USER_H_
#define _FF_USER_H_

#include "task/user_task.h"

namespace ff
{
class user_t
{
public:
    user_t(int64_t id_):m_uid(id_){}

    int64_t get_id() { return m_uid;}

    user_tasks_t& get_tasks() { return m_tasks; }
private:
    int64_t         m_uid;
    user_tasks_t    m_tasks;
};

};

#endif

