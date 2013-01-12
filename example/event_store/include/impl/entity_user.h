
#ifndef _ENTITY_USER_H
#define _ENTITY_USER_H

#include "entity.h"
#include "impl/event_def.h"
#include "event_store.h"

class entity_user_t: public entity_i
{
public:
    entity_user_t(uint64_t id_, event_store_i* event_store_);
    int inc_gold(int32_t gold_);
    int inc_level(int32_t level_);

    virtual int decode(const json_value_t& jval_);
    virtual string encode() const;
public:
    void apply(const inc_gold_event_t& event_);
    void apply(const inc_level_event_t& event_);
private:
    int32_t m_gold;
    int32_t m_level;
    event_store_i*  m_event_store;
};
#endif

