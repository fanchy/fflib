#include "net/text_socket_controller_impl.h"
#include "net/socket_i.h"
#include "base/strtool.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
using namespace std;

using namespace ff;

text_socket_controller_impl_t::text_socket_controller_impl_t(msg_handler_ptr_t msg_handler_):
    m_protocol(UNKNOWN_PROTOCOL),
    m_msg_handler(msg_handler_),
    m_head_end_flag(false),
    m_body_size(0)
{
}

text_socket_controller_impl_t::~text_socket_controller_impl_t()
{
}

//! when socket broken(whenever), this function will be called
//! this func just callback logic layer to process this event
//! each socket broken event only can happen once
//! logic layer has responsibily to deconstruct the socket object
int text_socket_controller_impl_t::handle_error(socket_i* sp_)
{
    m_msg_handler->handle_broken(sp_);
    return 0;
}

int text_socket_controller_impl_t::handle_read(socket_i* sp_, char* buff, size_t len)
{
    if (TXT_PROTOCOL == analyze_protocol(buff, len))
    {
        return parse_text_protocol(sp_, buff, len);
    }

    return parse_http_protocol(sp_, buff, len);
}

//! 当数据全部发送完毕后，此函数会被回调
//! 对于http1.0 协议, 发送完毕数据需要close掉连接
int text_socket_controller_impl_t::handle_write_completed(socket_i* sp_)
{
    //! only http1.0 supported
    if (HTTP_PROTOCOL == m_protocol)
    {
        sp_->close();
    }
    return 0;
}

int text_socket_controller_impl_t::parse_msg_head()
{
    vector<string> vt;
    strtool::split(m_head, vt, " ");

    size_t vt_size = vt.size();
    switch (vt_size)
    {
        case 1:
        {
            m_body_size = atoi(vt[0].c_str());;
        }
        break;
        case 2:
        case 3:
        {
            m_body_size = atoi(vt[0].c_str());;
        }
        break;
        default:
        {
            return -1;
        }
        break;
    }
    return 0;
}

int text_socket_controller_impl_t::append_msg_body(socket_i* sp_, char* buff_begin_, size_t& left_)
{
    size_t tmp        = m_body_size > left_ ? left_: m_body_size;
    left_             -= tmp;
    m_body_size  -= tmp;
    m_message.append_msg(buff_begin_, tmp);

    if (m_body_size == 0)
    {
        m_msg_handler->handle_msg(m_message, sp_);

        m_head_end_flag = false;
        m_head.clear();
        m_message.clear();
    }
    return tmp;
}

int text_socket_controller_impl_t::check_pre_send(socket_i*, string& buff_)
{
    ostringstream outstr;
    
    if (HTTP_PROTOCOL == m_protocol)
    {
        outstr <<  "HTTP/1.0 200 OK\r\nConnection: Close\r\nContent-type: text/html\r\nContent-length: "
               <<  buff_.size() << "\r\n\r\n" << buff_;
    }
    else if (TXT_PROTOCOL == m_protocol)
    {
        outstr << buff_.size() <<" \r\n" << buff_;
    }
    else
    {
        outstr << buff_.size() <<" \r\n" << buff_;
    }
    buff_ = outstr.str();
    return 0;
}

text_socket_controller_impl_t::protocol_e  text_socket_controller_impl_t::analyze_protocol(char* buff, size_t len)
{
    if (UNKNOWN_PROTOCOL != m_protocol)
    {
        return m_protocol;
    }
    
    if (false == m_head.empty())
    {
        if ('G' == m_head[0])//! http get request
        {
            m_protocol = HTTP_PROTOCOL;
        }
        else
        {
            m_protocol = TXT_PROTOCOL;
        }
    }
    else
    {
        if ('G' == buff[0])
        {
            m_protocol =  HTTP_PROTOCOL;
        }
        else
        {
            m_protocol = TXT_PROTOCOL;
        }
    }
    (void)len;
    return m_protocol;
}

int text_socket_controller_impl_t::parse_http_protocol(socket_i* sp_, char* buff, size_t len)
{
    char* buff_begin = buff;
    size_t left = len;

    if (false == m_head_end_flag)
    {
        char* pos = ::strstr(buff_begin, "\r\n");
        if (NULL == pos)
        {
            m_head.append(buff_begin, left);
            return 0;
        }

        m_head.append(buff_begin, pos - buff);
        vector<string> vt;
        
        strtool::split(m_head, vt, " ");

        if (vt.size() > 1)
        {
            m_message.append_to_body(vt[1].c_str(), vt[1].size());
        }

        m_head_end_flag = true;
        m_msg_handler->handle_msg(m_message, sp_);
    }
    return 0;
}

int text_socket_controller_impl_t::parse_text_protocol(socket_i* sp_, char* buff, size_t len)
{
    char* buff_begin = buff;
    size_t left = len;
    do
    {
        if (false == m_head_end_flag)
        {
            
            char* pos = ::strstr(buff_begin, "\r\n");
            if (NULL == pos)
            {
                m_head.append(buff_begin, left);
                return 0;
            }
            
            m_head.append(buff_begin, pos - buff);
            if (parse_msg_head())
            {
                sp_->async_send("ERROR\r\n");
                return -1;
            }
            
            m_head_end_flag = true;
            left -= (pos + 2 - buff_begin);
            buff_begin = pos + 2;
        }
        
        int consume = append_msg_body(sp_, buff_begin, left);
        buff_begin += consume;
    }
    while (left > 0);
    
    return 0;
}
