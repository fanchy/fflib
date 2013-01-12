#include "gateway_service.h"
#include "msg_bus.h"
#include "common_msg_def.h"
#include "rpc_callback.h"
#include "msg_sender.h"

using namespace ff;

gateway_service_t::gateway_service_t()
{
    
}

gateway_service_t::~gateway_service_t()
{
    
}

int gateway_service_t::handle_login(gate_msg_tool_t& msg_, socket_ptr_t sock_)
{
    struct lambda_t
    {
        static void callback(login_t::out_t& msg_, gateway_service_t* gs_, socket_ptr_t sock_)
        {
            //! send to client, add to gateway user map
            client_session_t* cs = new client_session_t();
            cs->uid = msg_.uid;
            sock_->set_data(cs);

            gs_->add_user(msg_.uid, sock_);
            msg_sender_t::send_to_client(sock_, msg_);
        }
    };
    singleton_t<msg_bus_t>::instance().get_service_group("manager")
                                      ->get_service(0)
                                      ->async_call(msg_, binder_t::callback(&lambda_t::callback, this, sock_));
    return 0;
}

int gateway_service_t::handle_logout(socket_ptr_t sock_)
{
    struct lambda_t
    {
        static void callback(logout_t::out_t& msg_)
        {
            //! del from gateway user map
        }
    };

    
    logout_t::in_t in;
    in.uid = sock_->get_data<client_session_t>()->uid;

    del_user(in.uid);
    singleton_t<msg_bus_t>::instance().get_service_group("manager")
                                      ->get_service(0)
                                      ->async_call(in, &lambda_t::callback);
    
    delete sock_->get_data<client_session_t>();
    sock_->set_data(NULL);
    return 0;
}

int gateway_service_t::handle_common_logic(gate_msg_tool_t& msg_, socket_ptr_t sock_)
{
    struct lambda_t
    {
        static void callback(common_msg_t::out_t& msg_, long uid_)
        {
            //! send to client, add to gateway user map
            //! msg_sender_t::send_to_client(sock_, msg_);
        }
    };
    long uid = sock_->get_data<client_session_t>()->uid;
    common_msg_t::in_t dest_msg;
    dest_msg.uid = uid;
    dest_msg.content = msg_.packet_body;
    singleton_t<msg_bus_t>::instance().get_service_group("logic")
                                        ->get_service(0)
                                        ->async_call(dest_msg, binder_t::callback(&lambda_t::callback, uid));
    return 0;
}

int gateway_service_t::add_user(long uid_, socket_ptr_t sock_)
{
    m_all_clients[uid_] = sock_;
    return 0;
}

int gateway_service_t::del_user(long uid_)
{
    m_all_clients.erase(uid_);
    return 0;
}

int gateway_service_t::unicast(unicast_t::in_t& msg_, rpc_callcack_t<unicast_t::out_t>& cb_)
{
    socket_ptr_t socket_ptr = m_all_clients[msg_.uid];
    msg_sender_t::send(socket_ptr, 0, msg_.content);
    
    unicast_t::out_t ret;
    cb_(ret);
    return 0;
}

int gateway_service_t::broadcast(broadcast_t::in_t& msg_, rpc_callcack_t<broadcast_t::out_t>& cb_)
{
    for (map<long, socket_ptr_t>::iterator it = m_all_clients.begin(); it != m_all_clients.end(); ++it)
    {
        msg_sender_t::send(it->second, 0, msg_.content);
    }
    
    broadcast_t::out_t ret;
    cb_(ret);
    return 0;
}
