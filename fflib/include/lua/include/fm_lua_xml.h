#ifndef _FM_LUA_XML_H_
#define _FM_LUA_XML_H_

#include "fm_lua.h"
#include "fm_json.h"
#include "array.h"
#include "xml_config_cache.h"

namespace fm_lua
{
    static Json::Value gen(xml_value_t& xv, const string& key_)
    {
        Json::Value obj;
        string cur_val = "{";
        for (size_t i = 0; i < xv.size(); ++ i)
        {
            string key = xv.get_child_tag_at(i);
            Json::Value value;
    
            xml_value_t cxv = xv.get_child_node_at(i);
            if (cxv.size() == 0)
            {
                value = Json::Value(xv.get_child_value_at(i));
                obj[key] = value;
            }
            else
            {
                value = gen(cxv, key);
                obj[key].append(value);
            }
            
        }

        return obj;
    }

    static string xml_to_json(const string& xml_)
    {
        xml_value_t xv;
        if (xv.parse_xml(xml_))
        {
            return "";
        }

        Json::Value root = gen(xv, "root");
        Json::FastWriter writer;
        return writer.write(root);
    }

    typedef xml_config_cache_t::pair_value_t          pair_value_t;
    typedef xml_config_cache_t::pair_value_array_t    pair_value_array_t;
    typedef xml_config_cache_t::config_result_array_t config_result_array_t;

    static config_result_array_t* parse_xml_to_struct(const string& xml_)
    {
        
        config_result_array_t* ret = singleton_t<xml_config_cache_t>::instance().alloc_xml_cache(xml_);
        if (ret)
        {
            return ret;
        }

        ret = new config_result_array_t();

        using namespace boost::property_tree;
        ptree root_pt;
        pair_value_array_t pv_array;
        try
        {
            read_xml(xml_, root_pt);
            ptree& data_pt = root_pt.get_child("data");
            ptree::iterator it = data_pt.begin();
            for (; it != data_pt.end(); ++it)
            {
                boost::property_tree::ptree& item_pt = it->second;
                ptree* attr_pt = NULL;
                try{
                    //! Œﬁ Ù–‘
                    attr_pt = &item_pt.get_child("<xmlattr>");
                }
                catch(exception& xx)
                {
                    continue;
                }
                ptree::iterator it2 = attr_pt->begin();
                
                for (; it2 != attr_pt->end(); ++it2)
                {
                    pair_value_t pv;
                    pv.key = it2->first.data();
                    pv.val = it2->second.data();
                    pv_array.push_back(pv);
                }
                ret->push_back(pv_array);
                pv_array.clear();
            }
        }
        catch(exception& e_)
        {
            cout <<"file<" << xml_ << "> parse failed:" << e_.what() <<"\n";
            return ret; 
        }
        return ret;
    }
};

LUA_REGISTER_BEGIN(fm_lua_xml)

REGISTER_CLASS_BASE("pair_value_t", fm_lua::pair_value_t, void())
REGISTER_CLASS_METHOD("pair_value_t", "get_key", fm_lua::pair_value_t, &fm_lua::pair_value_t::get_key)
REGISTER_CLASS_METHOD("pair_value_t", "get_val", fm_lua::pair_value_t, &fm_lua::pair_value_t::get_val)
    
REGISTER_CLASS_BASE("config_result_array_t", fm_lua::config_result_array_t, void())
REGISTER_CLASS_METHOD("config_result_array_t", "size", fm_lua::config_result_array_t, &fm_lua::config_result_array_t::size)
REGISTER_CLASS_METHOD("config_result_array_t", "at",   fm_lua::config_result_array_t, &fm_lua::config_result_array_t::at)

REGISTER_CLASS_BASE("pair_value_array_t", fm_lua::pair_value_array_t, void())
REGISTER_CLASS_METHOD("pair_value_array_t", "size", fm_lua::pair_value_array_t, &fm_lua::pair_value_array_t::size)
REGISTER_CLASS_METHOD("pair_value_array_t", "at",   fm_lua::pair_value_array_t, &fm_lua::pair_value_array_t::at)

REGISTER_STATIC_FUNCTION("fm_lua", "xml_to_json", fm_lua::xml_to_json)
REGISTER_STATIC_FUNCTION("fm_lua", "parse_xml_to_struct", fm_lua::parse_xml_to_struct)

LUA_REGISTER_END

#endif
