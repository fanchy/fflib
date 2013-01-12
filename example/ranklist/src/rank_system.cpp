
#include "rank_system.h"
#include "ranklist.h"

rank_system_t::rank_system_t(rank_obj_mgr_t* rank_obj_mgr_):
    m_rank_obj_mgr(rank_obj_mgr_)
{
    
}

rank_system_t::~rank_system_t()
{
    
}

ranklist_t& rank_system_t::create_ranklist(int attr_id_, int rank_num_)
{
    ranklist_map_t::iterator it = m_ranklist.find(attr_id_);
    if (it != m_ranklist.end())
    {
        return *(it->second);
    }
    ranklist_t* ret = new ranklist_t(m_rank_obj_mgr, attr_id_, rank_num_);
    m_ranklist[attr_id_] = ret;
    return *ret;
}

int rank_system_t::destory_ranklist(int attr_id_)
{
    ranklist_map_t::iterator it = m_ranklist.find(attr_id_);
    if (it != m_ranklist.end())
    {
        delete it->second;
        m_ranklist.erase(it);
    }
    return 0;
}
