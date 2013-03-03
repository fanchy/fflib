//! rpc 服务组
#ifndef _RPC_SERVICE_GROUP_H_
#define _RPC_SERVICE_GROUP_H_

#include <stdint.h>
#include <string>
#include <map>
using namespace std;

#include "rpc/rpc_service.h"

namespace ff {

class ffrpc_t;

class rpc_service_group_t
{
    typedef map<uint16_t, rpc_service_t*>   rpc_service_map_t;
public:
    rpc_service_group_t(const string& name_, uint16_t id_);
    ~rpc_service_group_t();

    uint16_t get_id() const;
    const string& get_name() const;

    rpc_service_t* get_service(uint16_t id_);

    int add_service(uint16_t id_, rpc_service_t* service_);

    size_t size() const { return m_all_rpc_service.size(); }
    
    template<typename T>
    void foreach(T func_);
private:
    uint16_t            m_id;
    string              m_name;
    rpc_service_map_t   m_all_rpc_service;
};


template<typename T>
void rpc_service_group_t::foreach(T func_)
{
    typename rpc_service_group_t::rpc_service_map_t::iterator it = m_all_rpc_service.begin();
    for (; it != m_all_rpc_service.end(); ++it)
    {
        func_(it->second);
    }
}

}
#endif
