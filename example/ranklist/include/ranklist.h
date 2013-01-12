#ifndef _RANKLIST_H_
#define _RANKLIST_H_

#include <map>
#include <vector>
using namespace std;

class rank_obj_t;
class rank_obj_mgr_t;

#include "rank_obj.h"

class ranklist_t
{
    typedef multimap<long, rank_obj_t*, greater<long> > sort_map_t;
	enum
	{
		MAX_RANK_NUM = 1000
	};
    struct sort_functor_t
    {
        sort_functor_t(int attr_id_, sort_map_t* sort_map_, int max_rank_num_):
            m_attr_id(attr_id_),
            m_sort_map(sort_map_),
			m_max_rank_num(max_rank_num_)
        {}
        void operator()(rank_obj_t* rank_obj_)
        {
			int value = rank_obj_->get_attr(m_attr_id);
			if (m_sort_map->size() <= (size_t)m_max_rank_num)
			{
				m_sort_map->insert(make_pair(value, rank_obj_));
			}
			else if (value > m_sort_map->rbegin()->second->get_attr(m_attr_id))
			{
				m_sort_map->insert(make_pair(value, rank_obj_));
                m_sort_map->erase(--(m_sort_map->end()));
			}
        }
        int         m_attr_id;
        sort_map_t* m_sort_map;
		int         m_max_rank_num;
    };

    struct rank_info_t
    {
        int                rank;
        rank_obj_t*        rank_obj;
        long               old_attr;
    };
public:
    ranklist_t(rank_obj_mgr_t* obj_mgr_, int attr_id_, int max_rank_num_ = MAX_RANK_NUM);

    void sort();
    int  update_obj(rank_obj_t* rank_obj_);

    int  get_rank(int from_, int to_, vector<rank_obj_t*>& ret_);

private:
    void resort_ranklist(int rank_, sort_map_t::iterator it_begin_, sort_map_t::iterator it_end_);
    sort_map_t::iterator find(long attr_, rank_obj_t* pbj_);

    int check_rank_num_limit();
    bool is_first(int rank_) const {return 1 == rank_;}
    bool is_last(int rank_) const  {return m_ranklist_cache_vt.size() == (size_t)rank_;}
private:
    rank_obj_mgr_t*             m_rank_obj_mgr;
    int                         m_attr_id;
	int							m_max_rank_num;
    sort_map_t                  m_ranklist_sort_map;

    //! rank 1- N
    vector<rank_info_t>        m_ranklist_cache_vt;
};
#endif
