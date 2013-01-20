//! rpc callback 声明
#ifndef _RPC_CALLBACK_H_
#define _RPC_CALLBACK_H_

#include <string>
using namespace std;

#include "net/socket_i.h"
#include "net/msg_sender.h"

namespace ff {

class rpc_callcack_base_t
{
public:
    rpc_callcack_base_t():
        m_socket(NULL),
        m_cmd(0)
    {}
    ~rpc_callcack_base_t(){}

    void operator()(const string& msg_)
    {
        exe(msg_);
    }
    void operator()(msg_i& msg_)
    {
        exe(msg_);
    }
    
    void exe(const string& msg_)
    {
        msg_sender_t::send(m_socket, rpc_msg_cmd_e::INTREFACE_CALLBACK, msg_);
    }
    
    void exe(msg_i& msg_)
    {
        if (0 != m_sgid)
        {
            msg_.set_sgid(m_sgid);
            msg_.set_sid(m_sid);
        }
        msg_.set_uuid(m_uuid);
        msg_sender_t::send(m_socket, rpc_msg_cmd_e::INTREFACE_CALLBACK, msg_);
    }

    void set_socket(socket_ptr_t socket_){  m_socket = socket_; }
    void set_cmd(uint16_t cmd_)          {  m_cmd = cmd_;   }
    socket_ptr_t get_socket() const      {  return m_socket;}
    
    void init_data(uint16_t cmd_, uint16_t sgid_, uint16_t sid_, uint32_t uuid_)
    {
        m_cmd = cmd_;
        m_sgid= sgid_;
        m_sid = sid_;
        m_uuid=uuid_;
    }
private:
    socket_ptr_t m_socket;
    uint16_t     m_cmd;
    uint16_t     m_sgid;
    uint16_t     m_sid;
    uint32_t     m_uuid;
};

template <typename MSGT>
struct rpc_callcack_t: public rpc_callcack_base_t
{
    void operator()(MSGT& msg_)
    {
        msg_.set_msg_id(singleton_t<msg_traits_t<MSGT> >::instance().get_id());
        rpc_callcack_base_t::exe(msg_);
    }
};

struct msg_process_func_i
{
    virtual ~msg_process_func_i(){}
    //virtual void exe(const string& msg_, rpc_callcack_t& cb_) = 0;
    virtual void exe(const string& msg_, uint16_t cmd_, socket_ptr_t socket_) = 0;
};

template <typename IN_MSG, typename RET, typename OUT_MSG>
struct msg_process_func_impl_t: public msg_process_func_i
{
    msg_process_func_impl_t(RET (*interface_)(IN_MSG&, rpc_callcack_t<OUT_MSG>&), uint16_t sgid_, uint16_t sid_):
        m_interface(interface_),
        sgid(sgid_),
        sid(sid_)
    {
    }
    virtual void exe(const string& msg_, uint16_t cmd_, socket_ptr_t socket_)
    {
        IN_MSG in_msg;
        in_msg.decode(msg_);

        rpc_callcack_t<OUT_MSG> cb;
        cb.init_data(cmd_, sgid, sid, in_msg.get_uuid());
        cb.set_socket(socket_);
    
        (*m_interface)(in_msg, cb);
    }
    RET (*m_interface)(IN_MSG&, rpc_callcack_t<OUT_MSG>&);
    uint16_t sgid;
    uint16_t sid;
};

template <typename IN_MSG, typename RET, typename T, typename OUT_MSG>
struct msg_process_class_func_impl_t: public msg_process_func_i
{
    msg_process_class_func_impl_t(RET (T::*interface_)(IN_MSG&, rpc_callcack_t<OUT_MSG>&), T* obj_, uint16_t sgid_, uint16_t sid_):
        m_interface(interface_),
        m_obj(obj_),
        sgid(sgid_),
        sid(sid_)
    {
    }
    virtual void exe(const string& msg_, uint16_t cmd_, socket_ptr_t socket_)
    {
        IN_MSG in_msg;
        in_msg.decode(msg_);
        rpc_callcack_t<OUT_MSG> cb;
        cb.init_data(cmd_, sgid, sid, in_msg.get_uuid());
        cb.set_socket(socket_);
        (m_obj->*(m_interface))(in_msg, cb);
    }
    RET (T::*m_interface)(IN_MSG&, rpc_callcack_t<OUT_MSG>&);
    T* m_obj;
    uint16_t sgid;
    uint16_t sid;
};

struct callback_wrapper_i
{
    virtual ~callback_wrapper_i() {}
    virtual void callback(const string& msg_buff_) = 0;
};

template <typename RET, typename MSGT>
struct callback_wrapper_cfunc_impl_t: public callback_wrapper_i
{
    callback_wrapper_cfunc_impl_t(RET (*callback_)(MSGT&)):
        m_callback(callback_)
    {}
    virtual void callback(const string& msg_buff_)
    {
        MSGT msg;
        try
        {
            msg.decode(msg_buff_);
        }
        catch (exception& e_){}
    
        (*m_callback)(msg);
    }
    RET (*m_callback)(MSGT&);
};

template <typename RET, typename MSGT, typename T>
struct callback_wrapper_class_impl_t: public callback_wrapper_i
{
    callback_wrapper_class_impl_t(RET (T::*func_)(MSGT&), T* obj_):
        m_callback(func_),
        m_obj(obj_)
    {}
    virtual void callback(const string& msg_buff_)
    {
        MSGT msg;
        try
        {
            msg.decode(msg_buff_);
        }
        catch (exception& e_){}

        (m_obj->*(m_callback))(msg);
    }
    RET (T::*m_callback)(MSGT&);
    T*  m_obj;
};

namespace binder_t
{
    template <typename RET, typename MSGT, typename ARG1>
    static callback_wrapper_i* callback(RET (*callback_)(MSGT&, ARG1), ARG1 arg1_)
    {
        struct callback_wrapper_cfunc_impl_t: public callback_wrapper_i
        {
            callback_wrapper_cfunc_impl_t(RET (*callback_)(MSGT&, ARG1), ARG1 arg1_):
                m_callback(callback_),
                m_arg1(arg1_)
            {}
            virtual void callback(const string& msg_buff_)
            {
                MSGT msg;
                msg.decode(msg_buff_);
                
                (*m_callback)(msg, m_arg1);
            }
            RET (*m_callback)(MSGT&, ARG1);
            ARG1 m_arg1;
        };
        return new callback_wrapper_cfunc_impl_t(callback_, arg1_);
    }

    template <typename RET, typename MSGT, typename ARG1, typename ARG2>
    static callback_wrapper_i* callback(RET (*callback_)(MSGT&, ARG1, ARG2), ARG1 arg1_, ARG2 arg2_)
    {
        struct callback_wrapper_cfunc_impl_t: public callback_wrapper_i
        {
            callback_wrapper_cfunc_impl_t(RET (*callback_)(MSGT&, ARG1, ARG2), ARG1 arg1_, ARG2 arg2_):
                m_callback(callback_),
                m_arg1(arg1_),
                m_arg2(arg2_)
            {}
            virtual void callback(const string& msg_buff_)
            {
                MSGT msg;
                msg.decode(msg_buff_);
                
                (*m_callback)(msg, m_arg1, m_arg2);
            }
            RET (*m_callback)(MSGT&, ARG1, ARG2);
            ARG1 m_arg1;
            ARG2 m_arg2;
        };
        return new callback_wrapper_cfunc_impl_t(callback_, arg1_, arg2_);
    }
    //!---------------------------------------- class function bind -----------
    
    template <typename RET, typename MSGT, typename T>
    static callback_wrapper_i* callback(RET (T::*callback_)(MSGT&), T* obj_)
    {
        return new callback_wrapper_class_impl_t<RET, MSGT, T>(callback_, obj_);
    }
    template <typename RET, typename MSGT, typename T, typename ARG1>
    static callback_wrapper_i* callback(RET (T::*callback_)(MSGT&, ARG1), T* obj_, ARG1 arg1_)
    {
        struct callback_wrapper_class_impl_t: public callback_wrapper_i
        {
            callback_wrapper_class_impl_t(RET (T::*func_)(MSGT&, ARG1), T* obj_, ARG1 arg1_):
            m_callback(func_),
            m_obj(obj_),
            m_arg1(arg1_)
            {}
            virtual void callback(const string& msg_buff_)
            {
                MSGT msg;
                msg.decode(msg_buff_);

                (m_obj->*(m_callback))(msg, m_arg1);
            }
            RET (T::*m_callback)(MSGT&, ARG1);
            T*  m_obj;
            ARG1 m_arg1;
        };
        return new callback_wrapper_class_impl_t(callback_, obj_, arg1_);
    }
    template <typename RET, typename MSGT, typename T, typename ARG1, typename ARG2>
    static callback_wrapper_i* callback(RET (T::*callback_)(MSGT&, ARG1, ARG2), T* obj_, ARG1 arg1_, ARG2 arg2_)
    {
        struct callback_wrapper_class_impl_t: public callback_wrapper_i
        {
            callback_wrapper_class_impl_t(RET (T::*func_)(MSGT&, ARG1, ARG2), T* obj_, ARG1 arg1_, ARG2 arg2_):
                m_callback(func_),
                m_obj(obj_),
                m_arg1(arg1_),
                m_arg2(arg2_)
            {}
            virtual void callback(const string& msg_buff_)
            {
                MSGT msg;
                msg.decode(msg_buff_);
                
                (m_obj->*(m_callback))(msg, m_arg1, m_arg2);
            }
            RET (T::*m_callback)(MSGT&, ARG1, ARG2);
            T*   m_obj;
            ARG1 m_arg1;
            ARG2 m_arg2;
        };
        return new callback_wrapper_class_impl_t(callback_, obj_, arg1_, arg2_);
    }
}

}
#endif
