
#ifndef _EVENT_STORE_MEM_H_
#define _EVENT_STORE_MEM_H_

#include <vector>
#include <map>
using namespace std;

#include "event_store.h"

class event_store_mem_t: public event_store_i
{
    typedef vector<string>                event_record_t;
    typedef map<uint64_t, event_record_t> entity_event_map_t;
    
    struct  snapshot_info_t
    {
        long   event_version;
        string data;
    };
    typedef vector<snapshot_info_t>       entity_record_t;
    typedef map<uint64_t, entity_record_t>entity_snapshot_map_t;
public:
    event_store_mem_t();
    ~ event_store_mem_t();
    
    int save_event(uint64_t entity_id_, const event_i& event_);
    int snapshot_entity(const entity_i& entity_);
    int constuct_snapshot_last(entity_i& entity_, event_dispather_i& event_dispacher_, int version_ = 1);

private:
    entity_event_map_t    m_entity_events;
    entity_snapshot_map_t m_entity_snapshot;
};
#endif

