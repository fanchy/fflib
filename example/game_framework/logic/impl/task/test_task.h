
#include "gtest/gtest.h"
#include "user_task.h"
#include "event_def.h"
#include "cqrs/bus_i.h"
using namespace ff;

#include <iostream>
using namespace std;

class task_event_counter_t
{
public:
    task_event_counter_t():task_accepted_counter(0){}
    void  handle(const task_accepted_t& e_)
    {
        task_accepted_counter ++;
    }
    int task_accepted_counter;
};

#define EVENT_COUNTER (singleton_t<task_event_counter_t>::instance())
TEST(TestTask, Accept) {
    user_tasks_t user_task;
    
    //! 先 mock出数据 只需给对象提供相应的event即可
    task_accepted_t e;
    e.task_id = 100;
    e.dest_value = 200;
    user_task.apply_change(e, false);

    //! test interface
    EXPECT_THROW(user_task.accet_task(100), user_tasks_t::task_exception_t);

    subscriber_t subscriber;
    subscriber.reg<task_accepted_t>(&EVENT_COUNTER);
    BUS.subscribe(subscriber);
    
    //! task_accepted_t will be trigger
    user_task.accet_task(200);
    EXPECT_TRUE(EVENT_COUNTER.task_accepted_counter == 1);
}

