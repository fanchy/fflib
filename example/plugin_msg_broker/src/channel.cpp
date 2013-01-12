#include "channel.h"

channel_t::channel_t(socket_ptr_t sock_):
    m_socket(sock_),
    m_data(NULL)
{
}

channel_t::~channel_t()
{
    if (m_socket)
    {
        m_socket->safe_delete();
    }
}

void  channel_t::set_data(void* p)
{
    m_data = p;
}

void* channel_t::get_data() const
{
    return m_data;
}

void channel_t::async_send(const string& buff_)
{
    m_socket->async_send(buff_);
}

void channel_t::close()
{
    m_socket->close();
}
