
#include "astar.h"
#include <iostream>
using namespace std;
using namespace ff;

astar_impl_t::astar_impl_t()
{
    
}

int astar_impl_t::init(uint32_t width_, uint32_t height_)
{
    m_width  = width_;
    m_height = height_;
    m_area.resize(width_ * height_);

    uint32_t index_val = 0;
    for (uint32_t i = 0; i < height_; ++i)
    {
        for (uint32_t j = 0; j < width_; ++j)
        {
            index_val = i * width_ + j;
            m_area[index_val] = CAN_PASS;
        }
    }
    return 0;
}

int astar_impl_t::change_pos_state(uint32_t index_, int flag_)
{
    m_area[index_] = pos_pass_flag_e(flag_);
    return 0;
}

int astar_impl_t::search_path(uint32_t from_x_, uint32_t from_y_, uint32_t to_x_, uint32_t to_y_, vector<pos_t>& path_)
{
    uint32_t from_index = from_x_  + from_y_ * m_width;
    //uint32_t to_index   = to_x_    + to_x_   * m_width;

    set<uint32_t>           close_table;
    map<uint32_t, pos_search_info_t> open_table;

    pos_search_info_t info;
    info.parent_index = 0;
    info.set_g_h(0, 0);
    info.set_pos(from_x_, from_y_, from_index);

    open_table[from_index] = info;
    close_table.insert(from_index);

    pos_search_info_t next = info;
    pos_search_info_t* tmp_min_f = NULL;
    pos_t pos;
    pos.x = from_x_;
    pos.y = from_y_;
    path_.push_back(pos);
    do {
        int begin_x = next.x - 1;
        int begin_y = next.y - 1;
        for (int j = begin_y; j < begin_y + 3; ++j)
        {
            for (int i = begin_x; i < begin_x + 3; ++i)
            {
                int tmp_index = j * m_width + i;
                if (i == (int)to_x_ && j == (int)to_y_)
                {
                    pos_t pos;
                    pos.x = i;
                    pos.y = j;
                    path_.push_back(pos);
                    return  0;
                }

                //! 该点不存在， 则忽略
                if ((i == (int)next.x && j == (int)next.y) ||
                    i < 0 || i > (int)m_width || j < 0 || j > (int)m_height ||
                    close_table.find(tmp_index) != close_table.end())
                {
                    continue;
                }
                
                cout <<"tmp_index:" << i << ":" << j << " " << tmp_index + 1 <<"\n";

                //! 该点若为障碍点，直接插入到close表 
                if (m_area[tmp_index] != CAN_PASS)
                {
                    close_table.insert(tmp_index);
                    continue;
                }
                
                //! 计算该点的g 和 h 值
                uint32_t g_val = next.g + distance(next.x, next.y, i, j);
                uint32_t h_val = distance(i, j, to_x_, to_y_);
                
                //!  构造节点信息
                pos_search_info_t node;
                node.parent_index = next.index;
                node.set_g_h(g_val, h_val);
                node.set_pos(i, j, tmp_index);
                
                pair<map<uint32_t, pos_search_info_t>::iterator, bool> ret =open_table.insert(make_pair(tmp_index, node));
                if (ret.second == false)
                {
                    if (node.f < ret.first->second.f)
                    {
                        //! 若采用新路径比原路径更近，则更新路径
                        ret.first->second.set_g_h(g_val, h_val);
                    }
                }
                if (NULL == tmp_min_f || node.f < tmp_min_f->f)
                {
                    tmp_min_f = &(ret.first->second);
                }
            }
        }
        
        if (NULL == tmp_min_f)
        {
            return  -1;
        }
        next = *tmp_min_f;
        cout <<"next:" << next.index + 1 <<"\n";
        open_table.erase(next.index);
        close_table.insert(next.index);
        pos_t pos;
        pos.x = next.x;
        pos.y = next.y;
        path_.push_back(pos);
    } while (1);
    return 0;
}

uint32_t astar_impl_t::distance(uint32_t from_x_, uint32_t from_y_, uint32_t to_x_, uint32_t to_y_)
{
    int  x_len = to_x_ - from_x_;
    if (x_len < 0) x_len = 0 - x_len;
    int y_len = to_y_ - from_y_;
    if (y_len < 0) y_len = 0 - y_len;
    
    return uint32_t(x_len + y_len);
}

