#ifndef _RANK_SYSTEM_H_
#define _RANK_SYSTEM_H_

class rank_obj_mgr_t;
class ranklist_t;

#include <map>
using namespace std;

class rank_system_t
{
    typedef map<int, ranklist_t*> ranklist_map_t;
public:
    rank_system_t(rank_obj_mgr_t* rank_obj_mgr_);
    ~rank_system_t();

    ranklist_t& create_ranklist(int attr_id_, int rank_num_);
    int destory_ranklist(int attr_id_);

protected:
    rank_obj_mgr_t*         m_rank_obj_mgr;
    ranklist_map_t          m_ranklist;
};

#endif
