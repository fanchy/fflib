#include "rpc/rpc_service_group.h"
#include "rpc/msg_bus.h"

using namespace ff;

rpc_service_group_t::rpc_service_group_t(msg_bus_t* mb_, const string& name_, uint16_t id_):
    m_id(id_),
    m_name(name_),
    m_msg_bus(mb_)
{}

rpc_service_group_t::~rpc_service_group_t()
{
    rpc_service_map_t::iterator it = m_all_rpc_service.begin();
    for (; it != m_all_rpc_service.end(); ++it)
    {
        delete it->second;
    }
    m_all_rpc_service.clear();
}

uint16_t rpc_service_group_t::get_id() const
{
    return m_id;
}

const string& rpc_service_group_t::get_name() const
{
    return m_name;
}

rpc_service_t* rpc_service_group_t::get_service(uint16_t id_)
{
    rpc_service_map_t::iterator it = m_all_rpc_service.find(id_);
    if (it != m_all_rpc_service.end())
    {
        return it->second;
    }
    return NULL;
}

int rpc_service_group_t::add_service(uint16_t id_, rpc_service_t* service_)
{
    return m_all_rpc_service.insert(make_pair(id_, service_)).second == true? 0: -1;
}

rpc_service_t& rpc_service_group_t::create_service(uint16_t id_)
{
    for (rpc_service_map_t::iterator it = m_all_rpc_service.begin(); it != m_all_rpc_service.end(); ++it)
    {
        if (id_ == it->second->get_id())
        {
            return *(it->second);
        }
    }
    
    m_msg_bus->register_service(get_name(), get_id(), id_);
    rpc_service_t* rs = new rpc_service_t(m_msg_bus, m_id, id_);
    this->add_service(id_, rs);
    return *rs;
}
