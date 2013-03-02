#include "rpc/rpc_service.h"
#include "rpc/ffrpc.h"
#include "log/log.h"

using namespace ff;

rpc_service_t::rpc_service_t(rpc_reg_i* mb_, uint16_t service_group_id_, uint16_t servie_id_):
    m_service_group_id(service_group_id_),
    m_service_id(servie_id_),
    m_uuid(0),
    m_bind_service_ptr(NULL),
    m_rpc_reg(mb_)
{
}

rpc_service_t::~rpc_service_t()
{
    for (interface_map_t::iterator it = m_interface_map.begin(); it != m_interface_map.end(); ++it)
    {
        delete it->second;
    }
    m_interface_map.clear();
}

uint16_t rpc_service_t::get_group_id() const
{
    return m_service_group_id;
}

uint16_t rpc_service_t::get_id() const
{
    return m_service_id;
}

socket_ptr_t rpc_service_t::get_socket() const
{
    return m_rpc_reg->get_socket(this);
}

void rpc_service_t::async_call(msg_i& msg_, uint16_t msg_id_, callback_wrapper_i* callback_)
{
    uint32_t uuid = (++ m_uuid);

    msg_.set(m_service_group_id, m_service_id, uuid, msg_id_);
    
    m_callback_map[uuid] = callback_;
    msg_sender_t::send(get_socket(), rpc_msg_cmd_e::CALL_INTERFACE, msg_);
    logtrace((RPC, "rpc_service_t::async_call m_service_id[%u], uuid[%u]", m_service_id, msg_.get_uuid()));
}

void rpc_service_t::async_call(gate_msg_tool_t& msg_, callback_wrapper_i* func_)
{
    uint16_t msg_id = singleton_t<msg_name_store_t>::instance().name_to_id(msg_.get_name());    
    this->async_call(msg_, msg_id, func_);
}
callback_wrapper_i* rpc_service_t::del_callback(uint32_t uuid_)
{
    callback_map_t::iterator it = m_callback_map.find(uuid_);
    if (it != m_callback_map.end())
    {
        callback_wrapper_i* ret = it->second;
        m_callback_map.erase(it);
        return ret;
    }
    return NULL;
}

msg_process_func_i* rpc_service_t::get_interface_func(uint32_t interface_id_)
{
    interface_map_t::const_iterator it = m_interface_map.find(interface_id_);
    if (it != m_interface_map.end())
    {
        return it->second;
    }
    return NULL;
}

int rpc_service_t::call_interface(msg_process_func_i* func_, const string& msg_buff_, socket_ptr_t socket_)
{
    rpc_callcack_base_t rcb;
    rcb.set_cmd(rpc_msg_cmd_e::INTREFACE_CALLBACK);
    rcb.set_socket(socket_);

    try
    {
        if (func_)
        {
            func_->exe(msg_buff_, rpc_msg_cmd_e::INTREFACE_CALLBACK, socket_);
            return 0;
        }
        rcb.exe("interface not existed");
    }
    catch (exception& e_)
    {
        rcb.exe(e_.what());
    }
    return -1;
}

int rpc_service_t::add_interface(const string& in_name_, const string& out_name_, msg_process_func_i* func_)
{
    uint16_t in_msg_id  = 0;
    uint16_t out_msg_id = 0;
    int ret = m_rpc_reg->register_interface(in_name_, out_name_, get_group_id(), get_id(), in_msg_id, out_msg_id);

    assert(ret == 0 && func_ && m_interface_map.insert(make_pair(in_msg_id, func_)).second == true  && "interface has existed");

    singleton_t<msg_name_store_t>::instance().add_msg(in_name_, in_msg_id);
    singleton_t<msg_name_store_t>::instance().add_msg(out_name_, out_msg_id);
    return 0; 
}
