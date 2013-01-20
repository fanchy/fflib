#ifndef _SOCKET_IMPL_H_
#define _SOCKET_IMPL_H_

#include <list>
#include <string>
using namespace std;

#include "net/socket_i.h"

namespace ff {

class epoll_i;
class socket_controller_i;
class task_queue_i;

#define  RECV_BUFFER_SIZE 8096

class socket_impl_t: public socket_i
{
public:
    typedef list<string>    send_buffer_t;

public:
    socket_impl_t(epoll_i*, socket_controller_i*, int fd, task_queue_i* tq_);
    ~socket_impl_t();

    virtual int socket() { return m_fd; }
    virtual void close();
    virtual void open();

    virtual int handle_epoll_read();
    virtual int handle_epoll_write();
    virtual int handle_epoll_del();

    virtual void async_send(const string& buff_);
    virtual void async_recv();
    virtual void safe_delete();
    
    int handle_epoll_read_impl();
    int handle_epoll_write_impl();
    int handle_epoll_error_impl();
    void send_impl(const string& buff_);
    void close_impl();
    
    socket_controller_i* get_sc() { return m_sc; }
private:
    bool is_open() { return m_fd > 0; }

    int do_send(const string& msg, string& left_);
private:
    epoll_i*                            m_epoll;
    socket_controller_i*                m_sc;
    int                                 m_fd;
    task_queue_i*                       m_tq;
    send_buffer_t                       m_send_buffer;
};

}

#endif
