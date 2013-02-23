#ifndef _HTTP_ACCEPTOR_H_
#define _HTTP_ACCEPTOR_H_

#include "net/acceptor_impl.h"
#include "net/common_socket_controller.h"
#include "net/net_stat.h"

namespace ff {

class http_acceptor_t: public acceptor_impl_t
{
public:
    http_acceptor_t(epoll_i*, msg_handler_i*, task_queue_pool_t* tq_);
    ~http_acceptor_t();

protected:
    virtual socket_i* create_socket(int new_fd_);

private:
};

}
#endif
