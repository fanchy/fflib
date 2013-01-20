
#include "net/gateway_acceptor.h"
#include "net/socket_impl.h"
#include "net/gateway_socket_controller.h"
#include "base/arg_helper.h"

using namespace ff;

gateway_acceptor_t::gateway_acceptor_t(epoll_i* e_, msg_handler_i* mh_, task_queue_pool_t* tq_):
    acceptor_impl_t(e_, mh_, tq_)
{
    
}

gateway_acceptor_t::~gateway_acceptor_t()
{
    
}

int gateway_acceptor_t::open(const string& args_)
{
    arg_helper_t arg_helper(args_);
    m_net_stat.start(arg_helper);

    return acceptor_impl_t::open(arg_helper.get_option_value("-gateway_listen"));
}

socket_i* gateway_acceptor_t::create_socket(int new_fd_)
{
    return new socket_impl_t(m_epoll, new gateway_socket_controller_t(m_msg_handler, &m_net_stat), new_fd_, m_tq->alloc(new_fd_));
}
