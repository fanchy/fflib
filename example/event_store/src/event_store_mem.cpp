#include "impl/event_store_mem.h"

event_store_mem_t::event_store_mem_t()
{
    
}

event_store_mem_t::~event_store_mem_t()
{
    
}

int event_store_mem_t::save_event(uint64_t entity_id_, const event_i& event_)
{
    m_entity_events[entity_id_].push_back(event_.encode());
    return 0;
}

int event_store_mem_t::snapshot_entity(const entity_i& entity_)
{
    snapshot_info_t info;
    info.event_version = m_entity_events[entity_.id()].size();
    info.data          = entity_.encode();
    m_entity_snapshot[entity_.id()].push_back(info);
    return 0;
}

int event_store_mem_t::constuct_snapshot_last(entity_i& entity_, event_dispather_i& event_dispacher_, int version_)
{
    int index = m_entity_snapshot.size() - version_;
    if (index >=0 && index < (int)m_entity_snapshot.size())
    {
        snapshot_info_t& info = m_entity_snapshot[entity_.id()][index];
        
        json_dom_t document;
        if (document.Parse<0>(info.data.c_str()).HasParseError())
        {
            throw msg_exception_t("json format not right");
        }
        if (false == document.IsObject() && false == document.Empty())
        {
            throw msg_exception_t("json must has one field");
        }

        entity_.decode(document.MemberBegin()->value);
        
        for (size_t i = info.event_version; i < m_entity_events[entity_.id()].size(); ++i)
        {
            event_dispacher_.dispath(m_entity_events[entity_.id()][i]);
        }
    }
    return 0;
}
