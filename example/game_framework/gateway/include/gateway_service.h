
#ifndef _GATEWAY_SERVICE_H_
#define _GATEWAY_SERVICE_H_

#include "common_msg_def.h"

#include "rpc/gateway_service_i.h"
#include "rpc/ffrpc.h"

namespace ff {

class gateway_service_t: public gateway_service_i
{
public:
    struct client_session_t
    {
        client_session_t():uid(0){}
        long uid;
    };
public:
    gateway_service_t();
    ~gateway_service_t();

    virtual int handle_login(gate_msg_tool_t& msg_, socket_ptr_t sock_);
    virtual int handle_logout(socket_ptr_t sock_);
    virtual int handle_common_logic(gate_msg_tool_t& msg_, socket_ptr_t sock_);

    int add_user(long uid_, socket_ptr_t sock_);
    int del_user(long uid_);
    
    //! gateway 如何定义接口给 logic server 和 manager server ？？
    virtual int unicast(unicast_t::in_t& msg_, rpc_callcack_t<unicast_t::out_t>& cb_);
    virtual int broadcast(broadcast_t::in_t& msg_, rpc_callcack_t<broadcast_t::out_t>& cb_);
private:
    map<long, socket_ptr_t>     m_all_clients;
};

}
#endif
