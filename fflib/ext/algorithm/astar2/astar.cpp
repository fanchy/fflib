
#include "astar.h"
#include <iostream>
using namespace std;
using namespace ff;

astar_t::astar_t(uint32_t width_, uint32_t height_):
    m_map(width_, height_)
{
}

int astar_t::search_path(uint32_t from_index, uint32_t to_index, vector<uint32_t>& path_)
{
    //! open 表中保存待扩展的节点
    //! visited 保存此次搜索访问过的节点，待搜索完成，将其状态恢复到默认状态
    open_table_t        open;
    vector<map_node_t*> visited;

    search_node_t current;
    search_node_t neighbor_node;
    vector<map_node_t*> neighbors;

    //! 先将起始点加入到open 表中
    current.set_pos_index(from_index);
    open.insert(current);

    visited.push_back(m_map.get_node(current.get_pos_index()));

    //! 大循环，直到open为空（找不到目标） 或 找到目标
    while (false == open.empty()) {
        open.pop_first(current);
        
        if (current.get_pos_index() == to_index)
        {
            break;
        }

        //! 添加到close 表
        m_map.get_node(current.get_pos_index())->set_closed();
    
        //! 找到当前节点的所有邻居节点， 不同的游戏中该函数实现可能不同
        //! 有的游戏可以走斜线，而有些不能，如坦克大战就不能走斜线
        m_map.get_neighbors(current.get_pos_index(), neighbors);

        for (size_t i = 0; i < neighbors.size(); ++i)
        {
            map_node_t* neighbor_map_node = neighbors[i];
            neighbor_node.set_pos_index(neighbor_map_node->get_pos_index());
            
            //! 计算该点的 g 和 h 值
            neighbor_node.set_gval(m_map.distance(current.get_pos_index(), neighbor_map_node->get_pos_index()));
            neighbor_node.set_hval(m_map.heuristic(neighbor_map_node->get_pos_index(), to_index));

            //! 如果该点已经在open表中，检查g值，若g值更小，说明当前路径更近，更新open表
            if (true == neighbor_map_node->is_open())
            {
                if (neighbor_node.get_gval() < neighbor_map_node->get_gval())
                {
                    open.update(neighbor_map_node->get_fval(), neighbor_node);
                    neighbor_map_node->set_gval(neighbor_node.get_gval());
                    neighbor_map_node->set_hval(neighbor_node.get_hval());
                    neighbor_map_node->set_parrent(current.get_pos_index());
                }
            }
            //! 如果该点既没有在open，也没有在close中，直接添加到open
            else if (false == neighbor_map_node->is_closed())
            {
                open.insert(neighbor_node);
                neighbor_map_node->set_open();
                neighbor_map_node->set_parrent(current.get_pos_index());
                visited.push_back(neighbor_map_node);
            }
            //! 如果已经在close 中，简单跳过
            else {} //! closed ignore
        }
        neighbors.clear();
    }

    //! 找到了目标，逆序遍历，得到完整的路径
    if (current.get_pos_index() == to_index)
    {
        path_.push_back(current.get_pos_index());
        uint32_t next = m_map.get_node(current.get_pos_index())->get_parrent();
        while (next != from_index)
        {
            path_.push_back(next);
            next = m_map.get_node(next)->get_parrent();
        }
        path_.push_back(from_index);
    }
    
    //! 最后将所有的已访问过的节点状态清楚, 为下次搜索做准备
    for (size_t i = 0; i < visited.size(); ++i)
    {
        visited[i]->clear();
    }
    visited.clear();
    return 0;
}

void astar_t::set_pos_pass_state(uint32_t pos_, bool flag_)
{
    m_map.set_pos_pass_state(pos_, flag_);
}

