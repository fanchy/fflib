#ifndef _BROKER_SERVICE_H_
#define _BROKER_SERVICE_H_

#include <map>
#include <set>
#include <string>
using namespace std;

#include "rpc/ffrpc.h"

namespace ff {

class broker_service_t: public rpc_reg_i
{
    struct socket_session_info_t
    {
        socket_session_info_t(uint16_t n_ = 0, string h_ = ""):
            node_id(n_),
            slave_host(h_)
        {}
        uint16_t node_id;
        string   slave_host;
    };
    struct proc_stack_t
    {
        uint64_t start_time;
        string   req_msg;
        uint32_t uuid;
        socket_ptr_t socket_ptr;
    };
    typedef map<uint32_t, proc_stack_t>           callback_map_t;
    typedef map<string, msg_process_func_i*>      interface_map_t;
    
    struct service_obj_t
    {
        service_obj_t():m_uuid(0){}
    
        string       name;
        uint16_t     group_id;
        uint16_t     id;
        socket_ptr_t socket_ptr;
        uint32_t     m_uuid;
        uint16_t     node_id;

        void async_call(msg_i& msg_, const string& body_, socket_ptr_t sp_);
        int interface_callback(msg_i& msg_, const string& body_);

        callback_map_t  m_callback_map;
        interface_map_t m_interface_map;
    };
    
    struct service_obj_mgr_t
    {
        uint16_t id;
        string   name;
        //socket_ptr_t socket_ptr;
        map<uint16_t, service_obj_t> service_objs;
    };
    typedef map<uint16_t, service_obj_mgr_t> service_obj_map_t;
public:
    broker_service_t();
    ~broker_service_t();

    int handle_broken(socket_ptr_t sock_);
    int handle_msg(const message_t& msg_, socket_ptr_t sock_);

    void create_service_group(create_service_group_t::in_t& in_msg_, rpc_callcack_t<create_service_group_t::out_t>& cb_);
    void create_service(create_service_t::in_t& in_msg_, rpc_callcack_t<create_service_t::out_t>& cb_);
    void reg_interface(reg_interface_t::in_t& in_msg_, rpc_callcack_t<reg_interface_t::out_t>& cb_);
    void sync_all_service(sync_all_service_t::in_t& in_msg_, rpc_callcack_t<sync_all_service_t::out_t>& cb_);
    void push_msg_except(socket_ptr_t socket_, uint16_t cmd_, msg_i& msg_);
    
    int  init_slave(const string& host_, const string& listen_host_);
    virtual socket_ptr_t get_socket(const rpc_service_t* rs_);
    
    socket_ptr_t find_socket_by_node(uint16_t node_id_);
    
    int process_sync_data(push_init_data_t::in_t& out);
private:
    mutex_t             m_mutex;
    service_obj_map_t   m_service_obj_mgr;
    uint32_t            m_uuid;
    uint32_t            m_msg_uuid;
    set<socket_ptr_t>   m_all_sockets;
    
    //! filed for conmunicate to master
    rpc_service_t*      m_master_broker;
    socket_ptr_t        m_master_socket;
    vector<string>      m_all_slave_host;
};

}
#endif
