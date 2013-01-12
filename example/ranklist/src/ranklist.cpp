#include "ranklist.h"
#include "rank_obj_mgr.h"

ranklist_t::ranklist_t(rank_obj_mgr_t* obj_mgr_, int attr_id, int max_rank_num_):
    m_rank_obj_mgr(obj_mgr_),
    m_attr_id(attr_id),
	m_max_rank_num(max_rank_num_)
{
}

void ranklist_t::sort()
{
    m_ranklist_sort_map.clear();
    m_ranklist_cache_vt.clear();

    sort_functor_t func(m_attr_id, &m_ranklist_sort_map, m_max_rank_num);
    m_rank_obj_mgr->foreach(func);

	resort_ranklist(1, m_ranklist_sort_map.begin(), m_ranklist_sort_map.end());
}

ranklist_t::sort_map_t::iterator ranklist_t::find(long attr_, rank_obj_t* obj_)
{
    pair<sort_map_t::iterator, sort_map_t::iterator> ret = m_ranklist_sort_map.equal_range(attr_);

    for (sort_map_t::iterator it=ret.first; it != ret.second; ++it)
    {
        if (it->second == obj_)
        {
            return it;
        }
    }
    return m_ranklist_sort_map.end();
}

int ranklist_t::update_obj(rank_obj_t* obj_)
{
    long now_attr = obj_->get_attr(m_attr_id);
    int old_rank  = obj_->get_rank();

    if (0 == old_rank)//! 还未加入排行榜
    {
        sort_map_t::iterator it_new = m_ranklist_sort_map.insert(make_pair(now_attr, obj_));
        int now_rank = 1;
        if (it_new != m_ranklist_sort_map.begin())
        {
            sort_map_t::iterator ItTmp = it_new;
            sort_map_t::iterator ItBefore = -- ItTmp;
            now_rank = ItBefore->second->get_rank() + 1;
        }
        resort_ranklist(now_rank, it_new, m_ranklist_sort_map.end());
        check_rank_num_limit();
        return -1;
    }

    //! 已经加入过排行榜,  检查排名是否发生变化
    //! 如果排名没有发生变化，直接返回
    
    //! 需要知道变化的开始、介绍的iterator, 以及从第几个排名开始变化
    long old_attr = m_ranklist_cache_vt[old_rank].old_attr;
    sort_map_t::iterator begin_change_it, end_change_it;
    int begin_change_rank = 1;

    if (now_attr >old_attr) //! 排名可能向前涨
    {
        if (is_first(old_rank) || now_attr <= m_ranklist_cache_vt[old_rank - 1].old_attr)
        {
            //!  排名不变
            return 0;
        }

        if (is_last(old_rank))//! 最后一名
        {
            sort_map_t::iterator tmp_it = find(old_attr, obj_);
            m_ranklist_sort_map.erase(tmp_it);
            begin_change_it = m_ranklist_sort_map.insert(make_pair(now_attr, obj_));
            end_change_it   = m_ranklist_sort_map.end();
        }
        else
        {
            rank_obj_t* next_obj = m_ranklist_cache_vt[old_rank + 1].rank_obj;
            sort_map_t::iterator tmp_it = find(old_attr, obj_);
            m_ranklist_sort_map.erase(tmp_it);
            begin_change_it = m_ranklist_sort_map.insert(make_pair(now_attr, obj_));
            end_change_it   = find(next_obj->get_attr(m_attr_id), next_obj);
        }
        //! 计算从第几个排名后开始发生变化
        if (begin_change_it == m_ranklist_sort_map.begin())
        {
            begin_change_rank = 1;
        }
        else
        {
            sort_map_t::iterator pre_it = begin_change_it;
            begin_change_rank = (++pre_it)->second->get_rank() + 1;
        }
    }
    else//! 排名可能向后退
    {
        if (is_last(old_rank) || now_attr >= m_ranklist_cache_vt[old_rank + 1].old_attr)
        {
            //!  排名不变
            return 0;
        }
        
        if (is_first(old_rank))//! 最后一名
        {
            sort_map_t::iterator tmp_it = find(old_attr, obj_);
            m_ranklist_sort_map.erase(tmp_it);
            end_change_it   = m_ranklist_sort_map.insert(make_pair(now_attr, obj_));
            ++ end_change_it;
            begin_change_it = m_ranklist_sort_map.begin();
            begin_change_rank = 1;
        }
        else
        {
            rank_obj_t* pre_obj = m_ranklist_cache_vt[old_rank - 1].rank_obj;
            sort_map_t::iterator tmp_it = find(old_attr, obj_);
            m_ranklist_sort_map.erase(tmp_it);

            end_change_it   = m_ranklist_sort_map.insert(make_pair(now_attr, obj_));
            ++ end_change_it;
            begin_change_it = find(pre_obj->get_attr(m_attr_id), pre_obj);
        }
        
        //! 计算从第几个排名后开始发生变化
        begin_change_rank = old_rank;
    }

    resort_ranklist(begin_change_rank, begin_change_it, end_change_it);
    return -1;
}

int  ranklist_t::get_rank(int from_, int to_, vector<rank_obj_t*>& ret_)
{
    int begin = (from_ > 0 && (size_t)from_ < m_ranklist_cache_vt.size())? from_: 0;
    int end   = (to_ > 0 && (size_t)to_ < m_ranklist_cache_vt.size())? to_: 0;
    end  =  end > begin? end: begin;

    for (int i = begin - 1; i < end; ++i)
    {
        ret_.push_back(m_ranklist_cache_vt[i].rank_obj);
    }
    return 0;
}

void ranklist_t::resort_ranklist(int rank_, sort_map_t::iterator it_begin_, sort_map_t::iterator it_end_)
{
    rank_info_t tmp_info;

    for (sort_map_t::iterator it = it_begin_; it != it_end_; ++it)
    {
        tmp_info.rank     = rank_++;
        tmp_info.rank_obj = it->second;
        tmp_info.old_attr = tmp_info.rank_obj->get_attr(m_attr_id);
        tmp_info.rank_obj->set_rank(tmp_info.rank);

        if (tmp_info.rank >= (int)m_ranklist_cache_vt.size())
        {
            m_ranklist_cache_vt.push_back(tmp_info);
        }
        else
        {
            m_ranklist_cache_vt[tmp_info.rank - 1] = tmp_info;
        }
    }
}

int ranklist_t::check_rank_num_limit()
{
    if (m_ranklist_cache_vt.size() <= (size_t)m_max_rank_num)
    {
        return 0;
    }
    m_ranklist_sort_map.erase(--(m_ranklist_sort_map.end()));
    m_ranklist_cache_vt.erase(m_ranklist_cache_vt.begin() + (m_ranklist_cache_vt.size() - 1));
    return -1;
}
