//! 消息发送管理
#ifndef _FF_FFRPC_H_
#define _FF_FFRPC_H_

#include <string>
#include <map>
#include <vector>
using namespace std;

#include "net/net_factory.h"
#include "rpc/rpc_service.h"
#include "rpc/rpc_service_group.h"
#include "rpc/rpc_future.h"
#include "base/signal_helper.h"
#include "rpc/rpc_reg.h"

namespace ff {

class ffrpc_t: public rpc_reg_i
{
    typedef map<uint16_t, rpc_service_group_t*>  service_map_t;
public:
    ffrpc_t();
    ~ffrpc_t();

    rpc_service_group_t& create_service_group(const string& name_);
    rpc_service_t&       create_service(const string& name_, uint16_t id_);

    rpc_service_group_t* get_service_group(uint16_t id_);
    rpc_service_group_t* get_service_group(const string& name_);
    rpc_service_t* get_service(const string& name_, uint16_t index_);
    
    int handle_broken(socket_ptr_t sock_);
    int handle_msg(const message_t& msg_, socket_ptr_t sock_);
    int process_sync_data(push_init_data_t::in_t& out);
    
    int open(const string& host_);
    int close();
    
    socket_ptr_t get_socket(const rpc_service_t* rs_);

    int register_service(const string& name_, uint16_t gid_, uint16_t id_);
    int register_interface(const string& in_name_, const string& out_name_, uint16_t gid_, uint16_t id_, uint16_t& in_alloc_id_, uint16_t& out_alloc_id_);
    
    template<typename T>
    int async_call(const string& service_name_, int index_, T& msg)
    {
        rpc_service_t* rs = NULL;
        {
            lock_guard_t lock(m_mutex);
            rpc_service_group_t* rsg = get_service_group(service_name_);
            if (rsg)
            {
                rs = rsg->get_service(index_);
            }
        }
        if (rs)
        {
            rs->async_call(msg);
            return 0;
        }
        return -1;
    }
    template<typename T, typename R>
    int async_call(const string& service_name_, int index_, T& msg, R cb_)
    {
        rpc_service_t* rs = NULL;
        {
            lock_guard_t lock(m_mutex);
            rpc_service_group_t* rsg = get_service_group(service_name_);
            if (rsg)
            {
                rs = rsg->get_service(index_);
            }
        }
        if (rs)
        {
            rs->async_call(msg, cb_);
            return 0;
        }
        return -1;
    }
    template<typename T, typename R>
    int call(const string& service_name_, int index_, T& msg, R& dest_)
    {
        rpc_service_t* rs = NULL;
        {
            lock_guard_t lock(m_mutex);
            rpc_service_group_t* rsg = get_service_group(service_name_);
            if (rsg)
            {
                rs = rsg->get_service(index_);
            }
        }
        if (rs)
        {
            rpc_future_t<R> rpc_future;
            dest_ = rpc_future.call(rs, msg);
            return 0;
        }
        return -1;
    }

    size_t service_num(const string& service_name_)
    {
        lock_guard_t lock(m_mutex);
        rpc_service_group_t* rsg = get_service_group(service_name_);
        if (rsg)
        {
            return rsg->size();
        }
        return 0;
    }
private:
    mutex_t             m_mutex;
    uint32_t            m_uuid;
    service_map_t       m_service_map;
    rpc_service_t*      m_broker_service;
    socket_ptr_t        m_socket;
    
    vector<socket_ptr_t>m_broker_slaves;
    //! 记录曾经使用过的broker连接
    vector<socket_ptr_t>m_history_sockets;
};
typedef ffrpc_t msg_bus_t;
}
#endif
