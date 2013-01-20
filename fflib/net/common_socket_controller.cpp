#include "net/common_socket_controller.h"
#include "net/socket_i.h"
#include "base/strtool.h"

#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
using namespace std;

using namespace ff;

common_socket_controller_t::common_socket_controller_t(msg_handler_ptr_t msg_handler_):
    m_msg_handler(msg_handler_),
    m_have_recv_size(0)
{
}

common_socket_controller_t::~common_socket_controller_t()
{
}

//! when socket broken(whenever), this function will be called
//! this func just callback logic layer to process this event
//! each socket broken event only can happen once
//! logic layer has responsibily to deconstruct the socket object
int common_socket_controller_t::handle_error(socket_i* sp_)
{
    m_msg_handler->handle_broken(sp_);
    return 0;
}

int common_socket_controller_t::handle_read(socket_i* sp_, char* buff, size_t len)
{
    size_t left_len = len;
    size_t tmp      = 0;
    
    while (left_len > 0)
    {
        if (false == m_message.have_recv_head(m_have_recv_size))
        {
            tmp = m_message.append_head(m_have_recv_size, buff, left_len);

            m_have_recv_size += tmp;
            left_len         -= tmp;
            buff             += tmp;
        }
        
        tmp = m_message.append_msg(buff, left_len);
        m_have_recv_size += tmp;
        left_len         -= tmp;
        buff             += tmp;
        
        if (m_message.get_body().size() == m_message.size())
        {
            m_msg_handler->handle_msg(m_message, sp_);
            m_have_recv_size = 0;
            m_message.clear();
        }
    }

    return 0;
}

//! 当数据全部发送完毕后，此函数会被回调
int common_socket_controller_t::handle_write_completed(socket_i* sp_)
{
    return 0;
}

int common_socket_controller_t::check_pre_send(socket_i* sp_, string& buff_)
{
    if (sp_->socket() < 0)
    {
        return -1;
    }
    return 0;
}
