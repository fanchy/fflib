#include "rpc/rpc_service_group.h"
#include "rpc/ffrpc.h"

using namespace ff;

rpc_service_group_t::rpc_service_group_t(const string& name_, uint16_t id_):
    m_id(id_),
    m_name(name_)
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
