#include "user_task.h"
#include "event_def.h"

using namespace ff;
#include <iostream>
using namespace std;

user_tasks_t::user_tasks_t()
{}

user_tasks_t::~user_tasks_t()
{}

void user_tasks_t::accet_task(int task_id_)
{
    if (m_tasks.find(task_id_) != m_tasks.end()) throw task_exception_t("tid exist");
    apply_change(task_accepted_t(task_id_, 100));
}

void user_tasks_t::complete_task(int task_id_)
{
    task_set_t::iterator it = m_tasks.find(task_id_);

    if (it == m_tasks.end()) throw task_exception_t("tid not exist");
    if (it->second.current_value < it->second.dest_value) throw task_exception_t("not completed");

    apply_change(task_completed_t(task_id_));
}

void user_tasks_t::modify_task(int task_id_, int value_)
{
    task_set_t::iterator it = m_tasks.find(task_id_);
    
    if (it == m_tasks.end()) throw task_exception_t("tid not exist");
    if (it->second.status != TASK_ACCEPTED) throw task_exception_t("status invalid");
    
    apply_change(task_modified_t(task_id_, value_));
}

void user_tasks_t::apply(const task_accepted_t& event_)
{
    task_ino_t task_info(event_.task_id, event_.dest_value, TASK_ACCEPTED);
    m_tasks.insert(make_pair(event_.task_id, task_info));
}

void user_tasks_t::apply(const task_completed_t& event_)
{
    m_tasks[event_.task_id].status = TASK_COMPLETED;
}

void user_tasks_t::apply(const task_modified_t& event_)
{
    m_tasks[event_.task_id].current_value += event_.value;
}


