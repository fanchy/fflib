#ifndef _TASK_QUEUE_IMPL_H_
#define _TASK_QUEUE_IMPL_H_

#include<pthread.h>
#include <list>
#include <stdexcept>
using namespace std;

#include "base/task_queue_i.h"
#include "base/lock.h"

namespace ff {

class task_queue_t: public task_queue_i
{
public:
    task_queue_t():
        m_flag(true),
        m_cond(m_mutex)
    {
    }
    ~task_queue_t()
    {
    }
    void close()
    {
    	lock_guard_t lock(m_mutex);
    	m_flag = false;
    	m_cond.broadcast();
    }

    void multi_produce(const task_list_t& task_)
    {
        lock_guard_t lock(m_mutex);
        bool need_sig = m_tasklist.empty();

        for(task_list_t::const_iterator it = task_.begin(); it != task_.end(); ++it)
        {
            m_tasklist.push_back(*it);
        }

        if (need_sig)
        {
        	m_cond.signal();
        }
    }
    void produce(const task_t& task_)
    {        
        lock_guard_t lock(m_mutex);
        bool need_sig = m_tasklist.empty();

        m_tasklist.push_back(task_);
        if (need_sig)
		{
			m_cond.signal();
		}
    }

    int   consume(task_t& task_)
    {
        lock_guard_t lock(m_mutex);
        while (m_tasklist.empty())
        {
            if (false == m_flag)
            {
                return -1;
            }
            m_cond.wait();
        }

        task_ = m_tasklist.front();
        m_tasklist.pop_front();

        return 0;
    }

    int run()
    {
        task_t t;
        while (0 == consume(t))
        {
            t.run();
        }
        return 0;
    }

    int consume_all(task_list_t& tasks_)
    {
        lock_guard_t lock(m_mutex);

        while (m_tasklist.empty())
        {
            if (false == m_flag)
            {
                return -1;
            }
            m_cond.wait();
        }

        tasks_ = m_tasklist;
        m_tasklist.clear();

        return 0;
    }

    int batch_run()
	{
    	task_list_t tasks;
    	int ret = consume_all(tasks);
		while (0 == ret)
		{
			for (task_list_t::iterator it = tasks.begin(); it != tasks.end(); ++it)
			{
				(*it).run();
			}
			tasks.clear();
			ret = consume_all(tasks);
		}
		return 0;
	}
private:
    volatile bool                   m_flag;
    task_list_t                     m_tasklist;
    mutex_t                         m_mutex;
    condition_var_t                 m_cond;
};

class task_queue_pool_t
{
	typedef task_queue_i::task_list_t task_list_t;
    typedef vector<task_queue_t*>    task_queue_vt_t;
    static void task_func(void* pd_)
    {
        task_queue_pool_t* t = (task_queue_pool_t*)pd_;
        t->run();
    }
public:
    static task_t gen_task(task_queue_pool_t* p)
    {
        return task_t(&task_func, p);
    }
public:
    task_queue_pool_t(int n):
    	m_index(0)
    {
        for (int i = 0; i < n; ++i)
        {
        	task_queue_t* p = new task_queue_t();
			m_tqs.push_back(p);
        }
    }

    void run()
    {
    	task_queue_t* p = NULL;
    	{
			lock_guard_t lock(m_mutex);
			if (m_index >= (int)m_tqs.size())
			{
				throw runtime_error("too more thread running!!");
			}
		    p = m_tqs[m_index++];
    	}

    	p->batch_run();
    }

    ~task_queue_pool_t()
    {
        task_queue_vt_t::iterator it = m_tqs.begin();
        for (; it != m_tqs.end(); ++it)
        {
            delete (*it);
        }
        m_tqs.clear();
    }

    void close()
    {
        task_queue_vt_t::iterator it = m_tqs.begin();
        for (; it != m_tqs.end(); ++it)
        {
            (*it)->close();
        }
    }

    size_t size() const { return m_tqs.size(); }
    
    task_queue_i* alloc(long id_)
    {
    	return m_tqs[id_ %  m_tqs.size()];
    }
    task_queue_i* rand_alloc()
	{
    	static unsigned long id_ = 0;
		return m_tqs[++id_ %  m_tqs.size()];
	}
private:
    mutex_t               m_mutex;
    task_queue_vt_t       m_tqs;
    int					  m_index;
};

}

#endif
