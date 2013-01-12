#include "impl/entity_user.h"

entity_user_t::entity_user_t(uint64_t id_, event_store_i* e_):
    entity_i(id_),
    m_gold(0),
    m_level(0),
    m_event_store(e_)
{
    event_dispather_t<entity_user_t> event_dispacher(*this);
    m_event_store->constuct_snapshot_last(*this, event_dispacher);
}

int entity_user_t::inc_gold(int32_t gold_)
{
    if (gold_ <= 0)
        return -1;
    inc_gold_event_t event(gold_);
    apply(event);
    m_event_store->save_event(this->id(), event);
    return 0;
}

int entity_user_t::inc_level(int32_t level_)
{
    if (level_ <= 0)
        return -1;
    inc_level_event_t event(level_);
    apply(event);
    m_event_store->save_event(this->id(), event);
    m_event_store->snapshot_entity(*this);
    return 0;
}

int entity_user_t::decode(const json_value_t& jval_)
{
    json_instream_t in("inc_gold_event_t");
    in.decode("m_gold", jval_["m_gold"], m_gold).decode("m_level", jval_["m_level"], m_level);
    return 0;
}

string entity_user_t::encode() const
{
    rapidjson::Document::AllocatorType allocator;
    rapidjson::StringBuffer            str_buff;
    json_value_t                       ibj_json(rapidjson::kObjectType);
    json_value_t                       ret_json(rapidjson::kObjectType);
    
    json_outstream_t out(allocator);
    out.encode("m_gold", ibj_json, m_gold).encode("m_level", ibj_json, m_level);
    ret_json.AddMember("entity_user_t", ibj_json, allocator);
    
    rapidjson::Writer<rapidjson::StringBuffer> writer(str_buff, &allocator);
    ret_json.Accept(writer);
    string output(str_buff.GetString(), str_buff.GetSize());
    return output;
}

void entity_user_t::apply(const inc_gold_event_t& event_)
{
    m_gold += event_.gold;
}

void entity_user_t::apply(const inc_level_event_t& event_)
{
    m_level += event_.level;
}
