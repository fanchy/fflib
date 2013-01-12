#ifndef _A_STAR_ELEMENT_H_
#define _A_STAR_ELEMENT_H_

#include <stdint.h>

class astar_element_t
{
public:
    void        set();
    uint32_t    get();

private:
    uint32_t    m_index_num;
};
#endif

