#ifndef _ACCEPTOR_IMPL_H_
#define _ACCEPTOR_IMPL_H_

#include "net/acceptor_i.h"

namespace ff {

#define LISTEN_BACKLOG 5

class epoll_i;
class socket_i;
class msg_handler_i;
class task_queue_pool_t;

class acceptor_impl_t: public acceptor_i
{
public:
    acceptor_impl_t(epoll_i*, msg_handler_i*, task_queue_pool_t* tq_);
    ~acceptor_impl_t();
    int   open(const string& address_);
    void close();

    int socket() {return m_listen_fd;}
    int handle_epoll_read();
    int handle_epoll_del() ;
    
protected:
    virtual socket_i* create_socket(int);

protected:
    int                 m_listen_fd;
    epoll_i*            m_epoll;
    msg_handler_i*      m_msg_handler;
    task_queue_pool_t*  m_tq;
};

}

#endif

