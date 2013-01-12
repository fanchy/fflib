#ifndef _EVENT_STORE_I_H_
#define _EVENT_STORE_I_H_

#include <stdint.h>

#include "event.h"
#include "entity.h"

class event_store_i
{
public:
    virtual ~event_store_i(){}

    virtual int save_event(uint64_t entity_id_, const event_i& event_) = 0;
    virtual int snapshot_entity(const entity_i& entity_)               = 0;
    virtual int constuct_snapshot_last(entity_i& entity_, event_dispather_i& event_dispacher_, int version_ = 1)               = 0;
};
#endif


