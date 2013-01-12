//! 消息发送管理
#ifndef _MSG_BUS_H_
#define _MSG_BUS_H_

#include <string>
#include <map>
#include <vector>
using namespace std;

#include "net_factory.h"
#include "rpc_service.h"
#include "rpc_service_group.h"
#include "rpc_future.h"
#include "utility/signal_helper.h"
#include "msg_bus_i.h"

namespace ff {

class msg_bus_t: public msg_bus_i
{
    typedef map<uint16_t, rpc_service_group_t*>  service_map_t;
public:
    msg_bus_t();
    ~msg_bus_t();

    rpc_service_group_t& create_service_group(const string& name_);
    rpc_service_t&       create_service(const string& name_, uint16_t id_);

    rpc_service_group_t* get_service_group(uint16_t id_);
    rpc_service_group_t* get_service_group(const string& name_);

    int handle_broken(socket_ptr_t sock_);
    int handle_msg(const message_t& msg_, socket_ptr_t sock_);

    int open(const string& host_);
    int close();
    
    socket_ptr_t get_socket(const rpc_service_t* rs_);

    int register_service(const string& name_, uint16_t gid_, uint16_t id_);
    int register_interface(const string& in_name_, const string& out_name_, uint16_t gid_, uint16_t id_, uint16_t& in_alloc_id_, uint16_t& out_alloc_id_);
    
private:
    mutex_t             m_mutex;
    uint32_t            m_uuid;
    service_map_t       m_service_map;
    rpc_service_t*      m_broker_service;
    socket_ptr_t        m_socket;
    
    vector<socket_ptr_t>m_broker_slaves;
};

}
#endif
