
#ifndef _A_STAR_H_
#define _A_STAR_H_

#include <stdint.h>
#include <vector>
#include <list>
#include <set>
#include <map>
using namespace std;

namespace ff {

class astar_t {
public:
    
};


class astar_impl_t {

    enum pos_pass_flag_e
    {
        CAN_PASS = 0,
        CAN_NOT_PASS
    };
public:
    struct pos_t
    {
        pos_t(): x(0), y(0){}
        uint32_t x;
        uint32_t y;
    };
    struct pos_search_info_t
    {
        void     set_g_h(uint32_t g_, uint32_t h_) { g = g_; h = h_; f = g + h; }
        void     set_pos(uint32_t x_, uint32_t y_, uint32_t index_) 
        { x = x_; y = y_; index = index_;}
        uint32_t index;
        uint32_t parent_index;
        uint32_t x;
        uint32_t y;
        uint32_t g;
        uint32_t h;
        uint32_t f;
    };
public:
    astar_impl_t();
    int init(uint32_t width_, uint32_t height_);

    int change_pos_state(uint32_t index_, int flag_);

    int search_path(uint32_t from_x_, uint32_t from_y_, uint32_t to_x_, uint32_t to_y_, vector<pos_t>& path_);

private:
    uint32_t distance(uint32_t from_x_, uint32_t from_y_, uint32_t to_x_, uint32_t to_y_);
private:
    uint32_t                   m_width;
    uint32_t                   m_height;
    vector<pos_pass_flag_e>    m_area;
};

}
#endif

