
#ifndef _MSG_BUS_I_
#define _MSG_BUS_I_

#include "net/msg_handler_i.h"
#include "net/net_factory.h"

namespace ff {

class msg_bus_i: public msg_handler_i
{
public:
    virtual ~msg_bus_i(){}

    virtual int register_interface(const string& in_name_, const string& out_name_, uint16_t gid_, uint16_t id_, uint16_t& in_alloc_id_, uint16_t& out_alloc_id_) { return -1; }
    
    virtual socket_ptr_t get_socket(const rpc_service_t* rs_) = 0;
};

}
#endif
