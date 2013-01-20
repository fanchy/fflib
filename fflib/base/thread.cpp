#include "base/thread.h"

using namespace ff;

void* thread_t::thread_func(void* p_)
{
    task_t* t = (task_t*)p_;
    t->run();
    delete  t;
    return 0;
}

int thread_t::create_thread(task_t func, int num)
{
    for (int i = 0; i < num; ++i)
    {
        pthread_t ntid;
        task_t* t = new task_t(func);
        if (0 == ::pthread_create(&ntid, NULL, thread_func, t))
        {
            m_tid_list.push_back(ntid);
        }
    }
    return 0;
}

int thread_t::join()
{
    list<pthread_t>::iterator it = m_tid_list.begin();
    for (; it != m_tid_list.end(); ++it)
    {
        pthread_join((*it), NULL);
    }
    m_tid_list.clear();
    return 0;
}
