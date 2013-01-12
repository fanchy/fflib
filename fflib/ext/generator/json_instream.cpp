#include "json_instream.h"

json_instream_t::json_instream_t(const char* struct_name_):
    m_struct_name(struct_name_)
{
}

json_instream_t& json_instream_t::decode(const char* filed_name_, const json_value_t& jval_, int8_t& dest_)
{
    if (false == jval_.IsInt())
    {
        snprintf(m_err_buff, sizeof(m_err_buff), "%s::%s[int8_t] field needed", m_struct_name, filed_name_);
        throw msg_exception_t(m_err_buff);
    }

    dest_ = jval_.GetInt();
    return *this;
}

json_instream_t& json_instream_t::decode(const char* filed_name_, const json_value_t& jval_, uint8_t& dest_)
{
    if (false == jval_.IsUint())
    {
        snprintf(m_err_buff, sizeof(m_err_buff), "%s::%s[uint8_t] field needed", m_struct_name, filed_name_);
        throw msg_exception_t(m_err_buff);
    }

    dest_ = jval_.GetUint();
    return *this;
}

json_instream_t& json_instream_t::decode(const char* filed_name_, const json_value_t& jval_, int16_t& dest_)
{
    if (false == jval_.IsInt())
    {
        snprintf(m_err_buff, sizeof(m_err_buff), "%s::%s[int16_t] field needed", m_struct_name, filed_name_);
        throw msg_exception_t(m_err_buff);
    }

    dest_ = jval_.GetInt();
    return *this;
}

json_instream_t& json_instream_t::decode(const char* filed_name_, const json_value_t& jval_, uint16_t& dest_)
{
    if (false == jval_.IsUint())
    {
        snprintf(m_err_buff, sizeof(m_err_buff), "%s::%s[uint16_t] field needed", m_struct_name, filed_name_);
        throw msg_exception_t(m_err_buff);
    }

    dest_ = jval_.GetUint();
    return *this;
}

json_instream_t& json_instream_t::decode(const char* filed_name_, const json_value_t& jval_, int32_t& dest_)
{
    if (false == jval_.IsInt())
    {
        snprintf(m_err_buff, sizeof(m_err_buff), "%s::%s[int32_t] field needed", m_struct_name, filed_name_);
        throw msg_exception_t(m_err_buff);
    }

    dest_ = jval_.GetInt();
    return *this;
}

json_instream_t& json_instream_t::decode(const char* filed_name_, const json_value_t& jval_, uint32_t& dest_)
{
    if (false == jval_.IsUint())
    {
        snprintf(m_err_buff, sizeof(m_err_buff), "%s::%s[int32_t] field needed", m_struct_name, filed_name_);
        throw msg_exception_t(m_err_buff);
    }

    dest_ = jval_.GetUint();
    return *this;
}

json_instream_t& json_instream_t::decode(const char* filed_name_, const json_value_t& jval_, int64_t& dest_)
{
    if (false == jval_.IsInt64())
    {
        snprintf(m_err_buff, sizeof(m_err_buff), "%s::%s[int64_t] field needed", m_struct_name, filed_name_);
        throw msg_exception_t(m_err_buff);
    }

    dest_ = jval_.GetInt64();
    return *this;
}

json_instream_t& json_instream_t::decode(const char* filed_name_, const json_value_t& jval_, uint64_t& dest_)
{
    if (false == jval_.IsUint64())
    {
        snprintf(m_err_buff, sizeof(m_err_buff), "%s::%s[uint64_t] field needed", m_struct_name, filed_name_);
        throw msg_exception_t(m_err_buff);
    }

    dest_ = jval_.GetUint64();
    return *this;
}

json_instream_t& json_instream_t::decode(const char* filed_name_, const json_value_t& jval_, bool& dest_)
{
    if (false == jval_.IsBool())
    {
        snprintf(m_err_buff, sizeof(m_err_buff), "%s::%s[bool] field needed", m_struct_name, filed_name_);
        throw msg_exception_t(m_err_buff);
    }

    dest_ = jval_.GetBool();
    return *this;
}

json_instream_t& json_instream_t::decode(const char* filed_name_, const json_value_t& jval_, float& dest_)
{
    if (false == jval_.IsDouble())
    {
        snprintf(m_err_buff, sizeof(m_err_buff), "%s::%s[float] field needed", m_struct_name, filed_name_);
        throw msg_exception_t(m_err_buff);
    }

    dest_ = (float)jval_.GetDouble();
    return *this;
}

json_instream_t& json_instream_t::decode(const char* filed_name_, const json_value_t& jval_, string& dest_)
{
    if (false == jval_.IsString())
    {
        snprintf(m_err_buff, sizeof(m_err_buff), "%s::%s[string] field needed", m_struct_name, filed_name_);
        throw msg_exception_t(m_err_buff);
    }

    dest_ = jval_.GetString();
    return *this;
}
