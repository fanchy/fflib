#include "logic_service.h"
#include "rpc/msg_bus.h"
#include "common_msg_def.h"
#include "logic_msg_def.h"
#include "rpc/rpc_callback.h"
#include "net/msg_sender.h"
#include "user/user_mgr.h"
#include "cqrs/bus_i.h"
using namespace ff;

logic_service_t::logic_service_t()
{
    
}

logic_service_t::~logic_service_t()
{
    
}

int logic_service_t::login(login_t::in_t& msg_, rpc_callcack_t<login_t::out_t>& cb_)
{
    USER_MGR.add_user(new user_t(msg_.uid));
    login_t::out_t ret;
    cb_(ret);

    struct lambda_t
    {
        static void callback(broadcast_t::out_t& msg_)
        {
        }
    };
    
    
    user_login_t notify_msg;
    notify_msg.uid = msg_.uid;

    broadcast_t::in_t broadcast;
    broadcast.content = notify_msg.encode();

    singleton_t<ffrpc_t>::instance().get_service_group("gateway")
                                        ->get_service(0)
                                        ->async_call(broadcast, &lambda_t::callback);

    return 0;
}

int logic_service_t::logout(logout_t::in_t& msg_, rpc_callcack_t<logout_t::out_t>& cb_)
{
    USER_MGR.del_user(msg_.uid);
    logout_t::out_t ret;
    cb_(ret);
    
    struct lambda_t
    {
        static void callback(logout_t::out_t& msg_)
        {
        }
    };
    
    user_logout_t notify_msg;
    notify_msg.uid = msg_.uid;
    
    broadcast_t::in_t broadcast;
    broadcast.content = notify_msg.encode();
    
    singleton_t<ffrpc_t>::instance().get_service_group("gateway")
                                        ->get_service(0)
                                        ->async_call(broadcast, &lambda_t::callback);
    return 0;
}

int logic_service_t::common_msg(common_msg_t::in_t& msg_, rpc_callcack_t<common_msg_t::out_t>& cb_)
{
    common_msg_t::out_t ret;
    cb_(ret);
    
    uint32_t* len = (uint32_t*)(msg_.content.c_str());
    string name(msg_.content.c_str()+4, *len);
    BUS.publish(name, msg_.content);
    return 0;
}


