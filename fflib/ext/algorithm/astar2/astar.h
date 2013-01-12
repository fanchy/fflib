
#ifndef _A_STAR_H_
#define _A_STAR_H_

#include <stdint.h>
#include <stdlib.h>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <queue>

using namespace std;

namespace ff {

class astar_t {

public:
    struct search_node_t
    {
        struct node_cmp_t
        {
            bool operator() (const search_node_t& a, const search_node_t& b )
            {
                return (a.get_gval() + a.get_hval()) > (b.get_gval() + b.get_hval());
            }
        };

        search_node_t():
            pos_index(0),
            gval(0),
            hval(0)
        {}

        bool operator ==(const search_node_t& node_) const
        {
            return this->get_pos_index() == node_.get_pos_index();
        }
        bool operator <(const search_node_t& node_) const
        {
            return this->get_fval() < node_.get_fval();
        }
        bool operator >(const search_node_t& node_) const
        {
            return this->get_fval() > node_.get_fval();
        }
        uint32_t get_pos_index() const { return pos_index; }
        uint32_t get_gval()      const { return gval; }
        uint32_t get_hval()      const { return hval; }
        uint32_t get_fval()      const { return gval + hval; }

        void     set_pos_index(uint32_t pos_)   { pos_index = pos_; }
        void     set_gval(uint32_t g_)          { gval = g_; }
        void     set_hval(uint32_t h_)          { hval = h_; }

        uint32_t pos_index;
        uint32_t gval;
        uint32_t hval;
    };
    typedef multimap<uint32_t, search_node_t> table_t;
    struct open_table_t
    {
        table_t nodes;
        bool empty() { return nodes.empty(); }
        int pop_first(search_node_t& ret)
        {
            table_t::iterator it = nodes.begin();
            ret = it->second;
            nodes.erase(it);
            return 0;
        }
        void insert(const search_node_t& node_)
        {
            nodes.insert(make_pair(node_.get_fval(), node_));
        }
        void update(uint32_t old_, const search_node_t& node_)
        {
            pair<table_t::iterator, table_t::iterator> ret = nodes.equal_range(old_);
            table_t::iterator it = ret.first;
            for (; it != ret.second; ++it)
            {
                if (it->second == node_)
                {
                    //! 可以优化， 如果前一个比该节点小，才需要删除
                    nodes.erase(it);
                }
            }
            this->insert(node_);
        }
    };

    struct map_node_t
    {
        enum node_state_e
        {
            NONE = 0,
            OPEN,
            CLOSED
        };

        map_node_t(uint32_t pos_):
            pos_index(pos_),
            parrent_pos_index(0),
            gval(0),
            flag(NONE),
            pass_flag(true)
        {}
        uint32_t get_pos_index() const { return pos_index;     }
        bool     is_open()       const { return OPEN == flag;  }
        bool     is_closed()     const { return CLOSED == flag;}
    
        void     set_open()   { flag = OPEN;   }
        void     set_closed() { flag = CLOSED; }
        void     set_none()   { flag = NONE;   }
    
        uint32_t get_gval() const      { return gval;}
        uint32_t get_hval() const      { return hval;}
        uint32_t get_fval() const      { return gval + hval; }
        void     set_gval(uint32_t g_) { gval = g_;  }
        void     set_hval(uint32_t h_) { hval = h_;  }
        
        void     set_parrent(uint32_t pos_)     {parrent_pos_index = pos_; }
        uint32_t get_parrent() const { return parrent_pos_index; }
        
        bool     is_can_pass() const { return pass_flag; }
        void     set_pass_flag(bool f_) { pass_flag = f_; }
        
        void     clear()
        {
            parrent_pos_index = 0;
            gval = 0;
            hval = 0;
            flag = NONE;
        }
        uint32_t      pos_index;
        uint32_t      parrent_pos_index;
        uint32_t      gval;
        uint32_t      hval;
        node_state_e  flag;
        bool          pass_flag;
    };
    struct map_mgr_t
    {
        map_mgr_t(uint32_t width_, uint32_t height_):
            m_map_nodes(NULL),
            m_width(width_),
            m_height(height_)
        {
            m_map_nodes = (map_node_t*)malloc(m_width * m_height * sizeof(map_node_t));
            for (uint32_t i = 0; i < m_height; ++i)
            {
                for (uint32_t j = 0; j < m_width; ++j)
                {
                    new(m_map_nodes + i * m_width + j) map_node_t(i * m_width + j);
                }
            }
        }
        ~map_mgr_t()
        {
            delete m_map_nodes;
            m_map_nodes = NULL;
        }
        map_node_t* get_node(uint32_t pos_) { return m_map_nodes + pos_; }
        void get_neighbors(uint32_t pos_, vector<map_node_t*>& ret_)
        {
            map_node_t* tmp = m_map_nodes + pos_ - 1;
            if (tmp >= m_map_nodes && tmp < m_map_nodes + m_height * m_width && tmp->is_can_pass())
            {
                ret_.push_back(tmp);
            }
            tmp = m_map_nodes + pos_ + 1;
            if (tmp >= m_map_nodes && tmp < m_map_nodes + m_height * m_width && tmp->is_can_pass())
            {
                ret_.push_back(tmp);
            }
            tmp = m_map_nodes + pos_ - m_width;
            if (tmp >= m_map_nodes && tmp < m_map_nodes + m_height * m_width && tmp->is_can_pass())
            {
                ret_.push_back(tmp);
            }
            tmp = m_map_nodes + pos_ + m_width;
            if (tmp >= m_map_nodes && tmp < m_map_nodes + m_height * m_width && tmp->is_can_pass())
            {
                ret_.push_back(tmp);
            }
        }
        uint32_t distance(uint32_t from_, uint32_t to_)
        {
            uint32_t m1 = from_ % m_width;
            uint32_t n1 = from_ / m_width;
            uint32_t m2 = to_ % m_width;
            uint32_t n2 = to_ / m_width;
            
            return ::abs(m1 -m2) + ::abs(n1 - n2);
        }
        uint32_t heuristic(uint32_t from_, uint32_t to_)
        {
            return this->distance(from_, to_);
        }
        void set_pos_pass_state(uint32_t pos_, bool flag_)
        {
            m_map_nodes[pos_].set_pass_flag(flag_);
        }
        
        map_node_t* m_map_nodes;
        uint32_t    m_width;
        uint32_t    m_height;
    };

public:
    astar_t(uint32_t width_, uint32_t height_);
    int search_path(uint32_t from_index, uint32_t to_index, vector<uint32_t>& path_);

    void set_pos_pass_state(uint32_t pos_, bool flag_);
private:
    map_mgr_t m_map;
};

}
#endif

