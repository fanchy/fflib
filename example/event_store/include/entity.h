#ifndef _ENTITY_I_H_
#define _ENTITY_I_H_

#include <stdint.h>
#include "serializer.h"

class entity_i: public serializer_i
{
public:
    entity_i(uint64_t id_):
        m_id(id_)
    {}
    virtual ~entity_i(){}
    uint64_t id() const { return m_id; }

protected:
    uint64_t m_id;
};
#endif

