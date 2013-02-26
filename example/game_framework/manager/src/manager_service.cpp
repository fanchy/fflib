#include "manager_service.h"
#include "rpc/ffrpc.h"
#include "common_msg_def.h"
#include "rpc/rpc_callback.h"
#include "net/msg_sender.h"

using namespace ff;

manager_service_t::manager_service_t()
{
    
}

manager_service_t::~manager_service_t()
{
    
}

int manager_service_t::login(login_t::in_t& msg_, rpc_callcack_t<login_t::out_t>& cb_)
{
    m_all_clients[msg_.uid];
    login_t::out_t ret;
    cb_(ret);
    
    struct lambda_t
    {
        static void callback(login_t::out_t& msg_)
        {
        }
    };
    
    singleton_t<ffrpc_t>::instance().get_service_group("logic")
                                        ->get_service(0)
                                        ->async_call(msg_, &lambda_t::callback);

    return 0;
}

int manager_service_t::logout(logout_t::in_t& msg_, rpc_callcack_t<logout_t::out_t>& cb_)
{
    m_all_clients.erase(msg_.uid);
    logout_t::out_t ret;
    cb_(ret);
    
    struct lambda_t
    {
        static void callback(logout_t::out_t& msg_)
        {
        }
    };
    
    singleton_t<ffrpc_t>::instance().get_service_group("logic")
                                        ->get_service(0)
                                        ->async_call(msg_, &lambda_t::callback);
    return 0;
}
