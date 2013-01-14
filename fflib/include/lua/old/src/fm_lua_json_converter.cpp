#include "fm_lua_json_converter.h"
//! 实现

fm_lua_json_converter_t::fm_lua_json_converter_t()
{
}

fm_lua_json_converter_t::~fm_lua_json_converter_t()
{
}

int fm_lua_json_converter_t::decode(const string& json_)
{
    try
    {
        Json::Value  root;
        Json::Reader reader;
        if(!reader.parse(json_, root, false))
        {    
            return -1;
        }

        if (true == root.isObject())
        {
            process_object(root);
        }
        else if(true == root.isArray())
        {
            process_array(root);
        }
        else
        {
            return -1;
        }
    }
    catch (exception& e_)
    {
        return -1;
    }
    return 0;
}

int fm_lua_json_converter_t::to_lua_table(lua_State* ls_)
{
    for (size_t i = 0; i < m_func_vt.size(); ++i)
    {
        m_func_vt[i](ls_);
    }
    return 0;
}

void fm_lua_json_converter_t::process_object(Json::Value& json_val_)
{
    string key;
    Json::Value::iterator it = json_val_.begin();

    //! 创建一个空table
    m_func_vt.push_back(boost::bind(&fm_lua_json_converter_t::lua_create_table, _1));

    for (; it != json_val_.end(); ++it)
    {
        key = (it).memberName();
        Json::Value val = json_val_[key];

        if (true == val.isInt())
        {
            m_func_vt.push_back(boost::bind(&fm_lua_json_converter_t::lua_set_table_string_int, _1, key, (int64_t)val.asInt()));
        }
        else if (true == val.isUInt())
        {
            m_func_vt.push_back(boost::bind(&fm_lua_json_converter_t::lua_set_table_string_uint, _1, key, (uint64_t)val.asUInt()));
        }
        else if (true == val.isDouble())
        {
            m_func_vt.push_back(boost::bind(&fm_lua_json_converter_t::lua_set_table_string_double, _1, key, val.asDouble()));
        }
        else if (true == val.isString())
        {
            m_func_vt.push_back(boost::bind(&fm_lua_json_converter_t::lua_set_table_string_string, _1, key, val.asString()));
        }
        else if (true == val.isBool())
        {
            m_func_vt.push_back(boost::bind(&fm_lua_json_converter_t::lua_set_table_string_bool, _1, key, val.asBool()));
        }
        else if (true == val.isObject())
        {
            m_func_vt.push_back(boost::bind(&fm_lua_json_converter_t::lua_set_table_string_key, _1, key));
            process_object(val);
            m_func_vt.push_back(&fm_lua_json_converter_t::lua_set_table_stack_arg);
        }
        else if (true == val.isArray())
        {
            m_func_vt.push_back(boost::bind(&fm_lua_json_converter_t::lua_set_table_string_key, _1, key));
            process_array(val);
            m_func_vt.push_back(&fm_lua_json_converter_t::lua_set_table_stack_arg);
        }      
    }
}

void fm_lua_json_converter_t::process_array(Json::Value& json_val_)
{
    //! 创建一个空table
    m_func_vt.push_back(boost::bind(&fm_lua_json_converter_t::lua_create_table, _1));

    int key = 0;
    for (size_t i = 0; i < json_val_.size(); ++i)
    {
        key = i + 1;
        Json::Value val = json_val_[i];

        if (true == val.isInt())
        {
            m_func_vt.push_back(boost::bind(&fm_lua_json_converter_t::lua_set_table_int_int, _1, key, (int64_t)val.asInt()));
        }
        else if (true == val.isUInt())
        {
            m_func_vt.push_back(boost::bind(&fm_lua_json_converter_t::lua_set_table_int_uint, _1, key, (uint64_t)val.asUInt()));
        }
        else if (true == val.isDouble())
        {
            m_func_vt.push_back(boost::bind(&fm_lua_json_converter_t::lua_set_table_int_double, _1, key, val.asDouble()));
        }
        else if (true == val.isString())
        {
            m_func_vt.push_back(boost::bind(&fm_lua_json_converter_t::lua_set_table_int_string, _1, key, val.asString()));
        }
        else if (true == val.isBool())
        {
            m_func_vt.push_back(boost::bind(&fm_lua_json_converter_t::lua_set_table_int_bool, _1, key, val.asBool()));
        }
        else if (true == val.isObject())
        {
            m_func_vt.push_back(boost::bind(&fm_lua_json_converter_t::lua_set_table_int_key, _1, key));
            process_object(val);
            m_func_vt.push_back(&fm_lua_json_converter_t::lua_set_table_stack_arg);
        }
        else if (true == val.isArray())
        {
            m_func_vt.push_back(boost::bind(&fm_lua_json_converter_t::lua_set_table_int_key, _1, key));
            process_array(val);
            m_func_vt.push_back(&fm_lua_json_converter_t::lua_set_table_stack_arg);
        }      
    }
}
