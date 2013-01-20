
#include "net/gateway_socket_controller.h"
#include "net/net_stat.h"

using namespace ff;

gateway_socket_controller_t::gateway_socket_controller_t(msg_handler_ptr_t msg_handler_, net_stat_t* ns_):
    common_socket_controller_t(msg_handler_),
    m_net_stat(ns_),
    m_last_update_tm(0)
{
    
}

gateway_socket_controller_t::~gateway_socket_controller_t()
{
    
}

int gateway_socket_controller_t::handle_open(socket_i* s_)
{
    m_last_update_tm = m_net_stat->get_heartbeat().tick();
    m_net_stat->get_heartbeat().add(s_);
    return 0;
}

int gateway_socket_controller_t::handle_read(socket_i* s_, char* buff, size_t len)
{
    common_socket_controller_t::handle_read(s_, buff, len);

    //! 判断消息包是否超过限制
    if (true == m_message.have_recv_head(m_have_recv_size) && m_message.size() > (size_t)m_net_stat->get_max_packet_size())
    {
        s_->close();
    }

    //! 更新心跳
    if (m_last_update_tm != m_net_stat->get_heartbeat().tick())
    {
        m_last_update_tm = m_net_stat->get_heartbeat().tick();
        m_net_stat->get_heartbeat().update(s_);
    }
    return 0;
}

int gateway_socket_controller_t::handle_error(socket_i* s_)
{
    m_net_stat->get_heartbeat().del(s_);
    return 0;
}
