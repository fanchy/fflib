#ifndef _PERFORMANCE_DEAMON_H_
#define _PERFORMANCE_DEAMON_H_

#include <string>
#include <map>
#include <fstream>
#include <iostream>
using namespace std;

#include "base/task_queue_impl.h"
#include "base/timer_service.h"

#include "base/thread.h"
#include "base/singleton.h"

namespace ff {

#define AUTO_PERF() performance_daemon_t::perf_tool_t __tmp__(__FUNCTION__)
#define PERF(m)     performance_daemon_t::perf_tool_t __tmp__(m)

//! 性能监控
class performance_daemon_t
{
public:
    struct perf_tool_t
    {
        perf_tool_t(const char* mod_):
            mod(mod_)
        {
            gettimeofday(&tm, NULL);
        }
        ~perf_tool_t()
        {
            struct timeval now;
            gettimeofday(&now, NULL);
            long cost = (now.tv_sec - tm.tv_sec)*1000000 + (now.tv_usec - tm.tv_usec);
            singleton_t<performance_daemon_t>::instance().post(mod, cost);
        }
        const char*    mod;
        struct timeval tm;
    };
    struct perf_info_t
    {
        perf_info_t():
            max(0),
            min(2147483647),
            total(0),
            times(0)
        {}
        long max;
        long min;
        long total;
        long times;
    };

    struct timer_lambda_t
    {
        static void exe(void* p_)
        {
            ((performance_daemon_t*)p_)->handle_timer();
        }
        static void setup_timer(void* p_)
        {
            performance_daemon_t* pd = (performance_daemon_t*)p_;
            pd->get_task_queue().produce(task_t(&timer_lambda_t::exe, pd));
        }
    };
public:
    performance_daemon_t();
    ~performance_daemon_t();

    //! 启动线程，创建文件
    int start(const string& path_, int seconds_);
    //! 关闭线程
    int stop();

    //! 增加性能监控数据
    void post(const string& mod_, long us);

    //! 更新性能日志
    void flush();

    //! 所有队列
    void run();

    task_queue_t& get_task_queue() { return m_task_queue; }
public:
    void add_perf_data(const string& mod_, long us);

private:
    void handle_timer();

private:
    volatile bool               m_started;
    int                         m_timeout_sec;
    map<string, perf_info_t>    m_perf_info;
    task_queue_t                m_task_queue;
    thread_t                    m_thread;
    ofstream                    m_fstream;
    timer_service_t*            m_timer_service;
};

}

#endif //PERFORMANCE_DEAMON_H_
