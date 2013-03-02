//!  rpc 异步服务
//! 接口
#ifndef _RPC_SERVICE_H_
#define _RPC_SERVICE_H_

#include <assert.h>
#include <map>
#include <string>
using namespace std;

#include "net/net_factory.h"
#include "rpc/rpc_callback.h"

namespace ff {

class ffrpc_t;
class rpc_reg_i;
 
struct callback_null_t
{
    static void callback(msg_tool_t& msg_){}
};
#define NULL_CB (&(callback_null_t::callback))

class rpc_service_t
{
public:
    typedef map<uint32_t, callback_wrapper_i*>      callback_map_t;
    typedef map<uint32_t, msg_process_func_i*>      interface_map_t;

    struct callback_guard_t
    {
        callback_guard_t(callback_wrapper_i* f_):
            cb(f_)
        {}
        ~callback_guard_t()
        {
            delete cb;
        }
        int exe(const string& buff_)
        {
            try
            {
                if (cb)
                {
                    cb->callback(buff_);
                    return 0;
                }
            }
            catch(exception& e_)
            {
                return -1;
            }
            return -1;
        }
        
        callback_wrapper_i* cb;
    };
public:
    rpc_service_t(rpc_reg_i* mb_, uint16_t service_group_id_, uint16_t servie_id_);
    virtual ~rpc_service_t();
    uint16_t get_group_id() const;
    uint16_t get_id() const;

    template<typename IN_MSG>
    void async_call(IN_MSG& msg_) { async_call(msg_, NULL_CB); }
    void async_call(msg_i& msg_, uint16_t msg_id_, callback_wrapper_i* callback_);
    template<typename RET, typename MSGT, typename IN_MSG>
    void async_call(IN_MSG& msg_, RET (*callback_)(MSGT&));
    template<typename IN_MSG>
    void async_call(IN_MSG& msg_, callback_wrapper_i* func_);

    //! special for gateway msg
    template<typename RET, typename MSGT>
    void async_call(gate_msg_tool_t& msg_, RET (*callback_)(MSGT&));
    void async_call(gate_msg_tool_t& msg_, callback_wrapper_i* func_);

    template <typename IN_MSG, typename RET, typename OUT_MSG>
    rpc_service_t& reg(RET (*interface_)(IN_MSG&, rpc_callcack_t<OUT_MSG>&));
    template <typename IN_MSG, typename RET, typename T, typename OUT_MSG>
    rpc_service_t& reg(RET (T::*interface_)(IN_MSG&, rpc_callcack_t<OUT_MSG>&), T* obj_);

    rpc_service_t& bind_service(void* p_) { m_bind_service_ptr = p_;  return *this; }
    template <typename IN_MSG, typename RET, typename T, typename OUT_MSG>
    rpc_service_t& reg(RET (T::*interface_)(IN_MSG&, rpc_callcack_t<OUT_MSG>&)) { return reg(interface_, (T*)m_bind_service_ptr); }

    callback_wrapper_i* del_callback(uint32_t uuid_);

    msg_process_func_i* get_interface_func(uint32_t interface_id_);
    int call_interface(msg_process_func_i* func_, const string& msg_buff_, socket_ptr_t sock_);
    
    virtual socket_ptr_t get_socket() const;

private:
    int add_interface(const string& in_name_, const string& out_name_, msg_process_func_i* func_);

private:
    uint16_t        m_service_group_id;
    uint16_t        m_service_id;
    uint32_t        m_uuid;
    callback_map_t  m_callback_map;
    interface_map_t m_interface_map;
    void*           m_bind_service_ptr;
    rpc_reg_i*      m_rpc_reg;
};

template<typename RET, typename MSGT, typename IN_MSG>
void rpc_service_t::async_call(IN_MSG& msg_, RET (*callback_)(MSGT&))
{
    uint16_t msg_id = singleton_t<msg_traits_t<IN_MSG> >::instance().get_id();

    this->async_call(msg_, msg_id, new callback_wrapper_cfunc_impl_t<RET, MSGT>(callback_));
}
template<typename IN_MSG>
void rpc_service_t::async_call(IN_MSG& msg_, callback_wrapper_i* func_)
{
    uint16_t msg_id = singleton_t<msg_traits_t<IN_MSG> >::instance().get_id();
    
    this->async_call(msg_, msg_id, func_);
}

template<typename RET, typename MSGT>
void rpc_service_t::async_call(gate_msg_tool_t& msg_, RET (*callback_)(MSGT&))
{
    uint16_t msg_id = singleton_t<msg_name_store_t>::instance().name_to_id(msg_.get_name());    
    this->async_call(msg_, msg_id, new callback_wrapper_cfunc_impl_t<RET, MSGT>(callback_));
}


template <typename IN_MSG, typename RET, typename OUT_MSG>
rpc_service_t& rpc_service_t::reg(RET (*interface_)(IN_MSG&, rpc_callcack_t<OUT_MSG>&))
{
    IN_MSG  in_msg;
    OUT_MSG out_msg;

    const string& in_msg_name  = in_msg.get_name();
    const string& out_msg_name = out_msg.get_name();

    msg_process_func_i* msg_process_func = new msg_process_func_impl_t<IN_MSG, RET, OUT_MSG>(interface_);

    this->add_interface(in_msg_name, out_msg_name, msg_process_func);
    return *this;
}

template <typename IN_MSG, typename RET, typename T, typename OUT_MSG>
rpc_service_t& rpc_service_t::reg(RET (T::*interface_)(IN_MSG&, rpc_callcack_t<OUT_MSG>&), T* obj_)
{
    IN_MSG  in_msg;
    OUT_MSG out_msg;
    
    const string& in_msg_name  = in_msg.get_name();
    const string& out_msg_name = out_msg.get_name();

    msg_process_func_i* msg_process_func = new msg_process_class_func_impl_t<IN_MSG, RET, T, OUT_MSG>(interface_, obj_,  m_service_group_id, m_service_id);

    assert(obj_);
    this->add_interface(in_msg_name, out_msg_name, msg_process_func);

    return *this;
}

}
#endif
