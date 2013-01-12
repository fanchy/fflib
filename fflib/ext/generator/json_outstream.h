#ifndef _JSON_OUTSTREAM_H_
#define _JSON_OUTSTREAM_H_

#include <stdint.h>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <sstream>
using namespace std;

#include "rapidjson/document.h"     // rapidjson's DOM-style API                                                                                             
#include "rapidjson/prettywriter.h" // for stringify JSON
#include "rapidjson/filestream.h"   // wrapper of C stream for prettywriter as output

typedef runtime_error       msg_exception_t;
typedef rapidjson::Document json_dom_t;
typedef rapidjson::Value    json_value_t;

class json_outstream_t
{
public:
    json_outstream_t(rapidjson::Document::AllocatorType& a);

    json_outstream_t& encode(const char* filed_name_, json_value_t& jval_, int8_t dest_);
    json_outstream_t& encode(const char* filed_name_, json_value_t& jval_, uint8_t dest_);
    json_outstream_t& encode(const char* filed_name_, json_value_t& jval_, int16_t dest_);
    json_outstream_t& encode(const char* filed_name_, json_value_t& jval_, uint16_t dest_);
    json_outstream_t& encode(const char* filed_name_, json_value_t& jval_, int32_t dest_);
    json_outstream_t& encode(const char* filed_name_, json_value_t& jval_, uint32_t dest_);
    json_outstream_t& encode(const char* filed_name_, json_value_t& jval_, int64_t dest_);
    json_outstream_t& encode(const char* filed_name_, json_value_t& jval_, uint64_t dest_);
    json_outstream_t& encode(const char* filed_name_, json_value_t& jval_, bool dest_);
    json_outstream_t& encode(const char* filed_name_, json_value_t& jval_, float dest_);
    json_outstream_t& encode(const char* filed_name_, json_value_t& jval_, const string& dest_);

    template<typename T>
    json_outstream_t& encode(const char* filed_name_, json_value_t& jval_, const T& dest_);
    template<typename T>
    json_outstream_t& encode(const char* filed_name_, json_value_t& jval_, const vector<T>& dest_);
    template<typename T, typename R>
    json_outstream_t& encode(const char* filed_name_, json_value_t& jval_, const map<T, R>& dest_);
    void to_json_val(const char* filed_name_, json_value_t& jval_, int8_t dest_) { jval_ = dest_; }
    void to_json_val(const char* filed_name_, json_value_t& jval_, uint8_t dest_) { jval_ = dest_; }
    void to_json_val(const char* filed_name_, json_value_t& jval_, int16_t dest_) { jval_ = dest_; }
    void to_json_val(const char* filed_name_, json_value_t& jval_, uint16_t dest_) { jval_ = dest_; }
    void to_json_val(const char* filed_name_, json_value_t& jval_, int32_t dest_) { jval_ = dest_; }
    void to_json_val(const char* filed_name_, json_value_t& jval_, uint32_t dest_) { jval_ = dest_; }
    void to_json_val(const char* filed_name_, json_value_t& jval_, const string& dest_) { jval_.SetString(dest_.c_str(), dest_.length(), m_allocator); }
    template<typename T>
    void to_json_val(const char* filed_name_, json_value_t& jval_, const T& dest_)
    {
        jval_.SetObject();
        dest_.encode_json_val(jval_, m_allocator);
    }

private:
    rapidjson::Document::AllocatorType& m_allocator;
};

template<typename T>
json_outstream_t& json_outstream_t::encode(const char* filed_name_, json_value_t& jval_, const T& dest_)
{
    json_value_t tmp_val(rapidjson::kObjectType);
    dest_.encode_json_val(tmp_val, m_allocator);
    jval_.AddMember(filed_name_, tmp_val, m_allocator);
    return *this;
}

template<typename T>
json_outstream_t& json_outstream_t::encode(const char* filed_name_, json_value_t& jval_, const vector<T>& dest_)
{
    json_value_t array_val(rapidjson::kArrayType);

    for (size_t i = 0; i < dest_.size(); ++i)
    {
        json_value_t tmp_val;
        to_json_val(filed_name_, tmp_val, dest_[i]);
        array_val.PushBack(tmp_val, m_allocator);
    }

    jval_.AddMember(filed_name_, array_val, m_allocator);
    return *this;
}

template<typename T, typename R>
json_outstream_t& json_outstream_t::encode(const char* filed_name_, json_value_t& jval_, const map<T, R>& dest_)
{
    json_value_t map_val(rapidjson::kObjectType);
    typename map<T, R>::const_iterator it = dest_.begin();
    for (; it != dest_.end(); ++it)
    {
        ostringstream ostr;
        ostr << it->first;

        string key = ostr.str();
        json_value_t tmp_val;
        //! this->encode(filed_name_, tmp_val, it->second);
        to_json_val(key.c_str(), tmp_val, it->second);

        map_val.AddMember(key.c_str(), tmp_val, m_allocator);
    }
    jval_.AddMember(filed_name_, map_val, m_allocator);
    return *this;
}

#endif
