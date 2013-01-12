#ifndef _EVENT_DEF_H_
#define _EVENT_DEF_H_

#include <stdexcept>
#include <iostream>
using namespace std;

#include "event.h"

struct inc_gold_event_t: public event_i
{
    inc_gold_event_t():
        gold(0)
    {}

    inc_gold_event_t(int32_t gold_):
        gold(gold_)
    {}
    int decode(const json_value_t& jval_)
    {
        json_instream_t in("inc_gold_event_t");
        in.decode("gold", jval_["gold"], gold);
        return 0;
    }
    string encode() const
    {
        rapidjson::Document::AllocatorType allocator;
        rapidjson::StringBuffer            str_buff;
        json_value_t                       ibj_json(rapidjson::kObjectType);
        json_value_t                       ret_json(rapidjson::kObjectType);
        
        json_outstream_t out(allocator);
        out.encode("gold", ibj_json, gold);
        ret_json.AddMember("inc_gold_event_t", ibj_json, allocator);
        
        rapidjson::Writer<rapidjson::StringBuffer> writer(str_buff, &allocator);
        ret_json.Accept(writer);
        string output(str_buff.GetString(), str_buff.GetSize());
        return output;
    }
    int32_t gold;
};

struct inc_level_event_t: public event_i
{
    inc_level_event_t():
        level(0)
    {}
    inc_level_event_t(int32_t level_):
    level(level_)
    {}
    int decode(const json_value_t& jval_)
    {
        json_instream_t in("inc_level_event_t");
        in.decode("level", jval_["level"], level);
        return 0;
    }
    string encode() const
    {
        rapidjson::Document::AllocatorType allocator;
        rapidjson::StringBuffer            str_buff;
        json_value_t                       ibj_json(rapidjson::kObjectType);
        json_value_t                       ret_json(rapidjson::kObjectType);
        
        json_outstream_t out(allocator);
        out.encode("level", ibj_json, level);
        ret_json.AddMember("inc_level_event_t", ibj_json, allocator);
        
        rapidjson::Writer<rapidjson::StringBuffer> writer(str_buff, &allocator);
        ret_json.Accept(writer);
        string output(str_buff.GetString(), str_buff.GetSize());
        return output;
    }
    int32_t level;
};

template<typename T>
class event_dispather_t: public event_dispather_i
{
    typedef int (event_dispather_t<T>::*reg_func_t)(const json_value_t&);
public:
    event_dispather_t(T& entity_):
        m_msg_handler(entity_)
    {
        
        m_reg_func["inc_gold_event_t"] = &event_dispather_t<T>::inc_gold_event_t_dispacher;
        m_reg_func["inc_level_event_t"] = &event_dispather_t<T>::inc_level_event_t_dispacher;
    }
    
    int dispath(const string& json_)
    {
        json_dom_t document;  // Default template parameter uses UTF8 and MemoryPoolAllocator.
        if (document.Parse<0>(json_.c_str()).HasParseError())
        {
            throw msg_exception_t("json format not right");
        }
        if (false == document.IsObject() && false == document.Empty())
        {
            throw msg_exception_t("json must has one field");
        }
        
        const json_value_t& val = document.MemberBegin()->name;
        const char* func_name   = val.GetString();
        typename map<string, reg_func_t>::const_iterator it = m_reg_func.find(func_name);
        
        if (it == m_reg_func.end())
        {
            char buff[512];
            snprintf(buff, sizeof(buff), "msg not supported<%s>", func_name);
            throw msg_exception_t(buff);
            return -1;
        }
        reg_func_t func = it->second;
        
        (this->*func)(document.MemberBegin()->value);
        return 0;
    }
    
    int inc_gold_event_t_dispacher(const json_value_t& jval_)
    {
        inc_gold_event_t event;
        event.decode(jval_);
        
        m_msg_handler.apply(event);
        return 0;
    }
    
    int inc_level_event_t_dispacher(const json_value_t& jval_)
    {
        inc_level_event_t event;
        event.decode(jval_);
        
        m_msg_handler.apply(event);
        return 0;
    }
    
private:
    T&                      m_msg_handler;
    map<string, reg_func_t> m_reg_func;
};

#endif

