#ifndef _GATEWAY_SERVICE_I_H_
#define _GATEWAY_SERVICE_I_H_

#include "net/codec.h"
#include "net/socket_i.h"
#include "net/msg_handler_i.h"

namespace ff {

class gateway_service_i: public msg_handler_i
{
public:
    virtual ~gateway_service_i() {}

    virtual int handle_login(gate_msg_tool_t& msg_, socket_ptr_t sock_)  = 0;
    virtual int handle_logout(socket_ptr_t sock_) = 0;
    virtual int handle_common_logic(gate_msg_tool_t& msg_, socket_ptr_t sock_) = 0;

    virtual int handle_broken(socket_ptr_t sock_)
    {
        handle_logout(sock_);
        sock_->safe_delete();
        return 0;
    }
    virtual int handle_msg(const message_t& msg_, socket_ptr_t sock_)
    {
        try
        {
            gate_msg_tool_t msg_tool;
            msg_tool.gate_decode(msg_.get_body());
            if (msg_tool.get_name() == "login_t::in_t")
            {
                handle_login(msg_tool, sock_);
            }
            else
            {
                handle_common_logic(msg_tool, sock_);
            }
        }
        catch (exception& e_)
        {
            sock_->close();
        }
        return 0;
    }
};

}
#endif
