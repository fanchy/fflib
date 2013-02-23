
#include "net/http_acceptor.h"
#include "net/socket_impl.h"
#include "net/text_socket_controller_impl.h"
#include "base/arg_helper.h"

using namespace ff;

http_acceptor_t::http_acceptor_t(epoll_i* e_, msg_handler_i* mh_, task_queue_pool_t* tq_):
    acceptor_impl_t(e_, mh_, tq_)
{
    
}

http_acceptor_t::~http_acceptor_t()
{
    
}

socket_i* http_acceptor_t::create_socket(int new_fd_)
{
    return new socket_impl_t(m_epoll, new text_socket_controller_impl_t(m_msg_handler), new_fd_, m_tq->alloc(new_fd_));
}
