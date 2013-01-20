
#include "net/net_stat.h"
using namespace ff;

net_stat_t::net_stat_t():
    m_timer_service(NULL),
    m_max_packet_size(1000)
{
}

net_stat_t::~net_stat_t()
{
    if (m_timer_service)
    {
        delete m_timer_service;
        m_timer_service = NULL;
    }
}

static void timer_close(socket_ptr_t s_)
{
    s_->close();
}

static void timer_check(void* p_)
{
    ((net_stat_t*)p_)->handle_timer_check();
}

int net_stat_t::start(arg_helper_t& arg_helper_)
{
    if (arg_helper_.is_enable_option("-max_packet_size"))
    {
        m_max_packet_size = ::atoi(arg_helper_.get_option_value("-max_packet_size").c_str());
    }
    
    m_heartbeat.set_option(arg_helper_, timer_close);
    
    m_timer_service = new timer_service_t(1); //! 1s tick
    
    m_timer_service->timer_callback(m_heartbeat.timeout(), task_t(&timer_check, (void*)this));
    return 0;
}

void net_stat_t::handle_timer_check()
{
    m_heartbeat.timer_check();
    if (m_timer_service)
    {
        m_timer_service->timer_callback(m_heartbeat.timeout(), task_t(&timer_check, (void*)this));
    }
}

int net_stat_t::stop()
{
    if (m_timer_service)
    {
        delete m_timer_service;
        m_timer_service = NULL;
    }
    return 0;
}

