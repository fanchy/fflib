#include "json_outstream.h"

json_outstream_t::json_outstream_t(rapidjson::Document::AllocatorType& a):
    m_allocator(a)
{
}

json_outstream_t& json_outstream_t::encode(const char* filed_name_, json_value_t& jval_, int8_t dest_)
{
    jval_.AddMember(filed_name_, dest_, m_allocator);
    return *this;
}

json_outstream_t& json_outstream_t::encode(const char* filed_name_, json_value_t& jval_, uint8_t dest_)
{
    jval_.AddMember(filed_name_, dest_, m_allocator);
    return *this;
}

json_outstream_t& json_outstream_t::encode(const char* filed_name_, json_value_t& jval_, int16_t dest_)
{
    jval_.AddMember(filed_name_, dest_, m_allocator);
    return *this;
}

json_outstream_t& json_outstream_t::encode(const char* filed_name_, json_value_t& jval_, uint16_t dest_)
{
    jval_.AddMember(filed_name_, dest_, m_allocator);
    return *this;
}

json_outstream_t& json_outstream_t::encode(const char* filed_name_, json_value_t& jval_, int32_t dest_)
{
    jval_.AddMember(filed_name_, dest_, m_allocator);
    return *this;
}

json_outstream_t& json_outstream_t::encode(const char* filed_name_, json_value_t& jval_, uint32_t dest_)
{
    jval_.AddMember(filed_name_, dest_, m_allocator);
    return *this;
}

json_outstream_t& json_outstream_t::encode(const char* filed_name_, json_value_t& jval_, int64_t dest_)
{
    jval_.AddMember(filed_name_, dest_, m_allocator);
    return *this;
}

json_outstream_t& json_outstream_t::encode(const char* filed_name_, json_value_t& jval_, uint64_t dest_)
{
    jval_.AddMember(filed_name_, dest_, m_allocator);
    return *this;
}

json_outstream_t& json_outstream_t::encode(const char* filed_name_, json_value_t& jval_, bool dest_)
{
    jval_.AddMember(filed_name_, dest_, m_allocator);
    return *this;
}

json_outstream_t& json_outstream_t::encode(const char* filed_name_, json_value_t& jval_, float dest_)
{
    jval_.AddMember(filed_name_, dest_, m_allocator);
    return *this;
}

json_outstream_t& json_outstream_t::encode(const char* filed_name_, json_value_t& jval_, const string& dest_)
{
    json_value_t tmp_val(dest_.c_str(), dest_.length(), m_allocator);
    jval_.AddMember(filed_name_, tmp_val, m_allocator);
    return *this;
}
