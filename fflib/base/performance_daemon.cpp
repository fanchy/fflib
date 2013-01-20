#include "base/performance_daemon.h"

using namespace ff;

performance_daemon_t::performance_daemon_t():
m_started(false),
m_timeout_sec(3600),
m_timer_service(NULL)
{
}

performance_daemon_t::~performance_daemon_t()
{
    stop();
}


int performance_daemon_t::start(const string& path_, int seconds_)
{
    if (true == m_started) return -1;
    
    m_timeout_sec = seconds_;
    
    //! 启动定时器 1 times/seconds
    m_timer_service = new timer_service_t(1000);
    
    m_started = true; //! timer will start
    
    struct lambda_t
    {
        static void run(void* p_)
        {
            ((performance_daemon_t*)p_)->run();
        }
    };
    
    m_fstream.open(path_.c_str());
    m_thread.create_thread(task_t(&lambda_t::run, this), 1);
    return 0;
}

int performance_daemon_t::stop()
{
    if (false == m_started) return -1;
    
    if (m_timer_service)
    {
        delete m_timer_service;
        m_timer_service = NULL;
    }
    
    m_started = false; //! timer will stop
    
    m_task_queue.close();
    m_thread.join();
    m_fstream.close();
    return 0;
}

void performance_daemon_t::post(const string& mod_, long us_)
{
    struct lambda_perf_data_t
    {
        static void add_perf_data(void* p_)
        {
            lambda_perf_data_t* pd_ptr = (lambda_perf_data_t*)p_;
            pd_ptr->pd->add_perf_data(pd_ptr->mod, pd_ptr->cost);
            
            delete pd_ptr;
        }
        lambda_perf_data_t(performance_daemon_t* p_, const string& mod_, long cost_):
        pd(p_),
        mod(mod_),
        cost(cost_)
        {
        }
        performance_daemon_t* pd;
        string                mod;
        long                  cost;
    };
    
    if (m_started)
    {
        m_task_queue.produce(task_t(&lambda_perf_data_t::add_perf_data, new lambda_perf_data_t(this, mod_, us_)));
    }
    else
    {
        this->start("perf.txt", 60*10);
    }
}

void performance_daemon_t::handle_timer()
{
    flush();
    m_perf_info.clear();
    m_timer_service->timer_callback(m_timeout_sec * 1000, task_t(&timer_lambda_t::setup_timer, this));
}

void performance_daemon_t::flush()
{
    map<string, perf_info_t>::iterator it = m_perf_info.begin();
    
    time_t timep   = time(NULL);
    struct tm *tmp = localtime(&timep);
    
    char tmp_buff[256];
    sprintf(tmp_buff, "%04d%02d%02d-%02d:%02d:%02d",
            tmp->tm_year + 1900, tmp->tm_mon + 1, tmp->tm_mday,
            tmp->tm_hour, tmp->tm_min, tmp->tm_sec);
    char buff[1024] = {0};
    
    snprintf(buff, sizeof(buff), "time,mod,max_cost[us],min_cost[us],per_cost[us],request_per_second,exe_times\n");
    m_fstream << buff;
    
    for (; it != m_perf_info.end(); ++it)
    {
        perf_info_t& pinfo = it->second;
        long per = pinfo.total / pinfo.times;
        long rps = (per == 0? -1: 1000000 / per);
        
        //! -------------------------- time, mod, max, min, per, rps, times
        snprintf(buff, sizeof(buff), "%s,%s,%ld,%ld,%ld,%ld,%ld\n",
                 tmp_buff, it->first.c_str(), pinfo.max, pinfo.min, per, rps, pinfo.times);
        m_fstream << buff;
    }
    
    m_fstream.flush();
}



void performance_daemon_t::run()
{
    m_timer_service->timer_callback(m_timeout_sec * 1000, task_t(&timer_lambda_t::setup_timer, this));
    m_task_queue.run();
}

void performance_daemon_t::add_perf_data(const string& mod_, long us_)
{
    perf_info_t& pinfo = m_perf_info[mod_];
    
    pinfo.total += us_;
    pinfo.times += 1;
    
    if (us_ > pinfo.max)
    {
        pinfo.max = us_;
    }
    else if (us_ < pinfo.min)
    {
        pinfo.min = us_;
    }
}
