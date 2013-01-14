#ifndef _XML_CONFIG_CACHE_H_
#define _XML_CONFIG_CACHE_H_

#include <boost/thread.hpp>
#include <boost/property_tree/ptree.hpp>    
#include <boost/property_tree/xml_parser.hpp>
#include <boost/typeof/typeof.hpp>

#include <string>
#include <map>
using namespace std;

#include "array.h"
#include "singleton.h"

class xml_config_cache_t
{
    typedef boost::mutex                mutex_t;
    typedef boost::mutex::scoped_lock   scoped_lock_t;
public:
    struct pair_value_t
    {
        string get_key() const { return key; }
        string get_val() const { return val; }
        string key;
        string val;
    };
    typedef array_t<pair_value_t>         pair_value_array_t;
    typedef array_t<pair_value_array_t>   config_result_array_t;

public:
    int cache_xml(const string& xml_)
    {
        config_result_array_t* ret = NULL;
        {
            scoped_lock_t lock(m_mutex);
            if (m_xml_cache_store.find(xml_) != m_xml_cache_store.end())
            {
                //! exist
                return 0;
            }
            ret = new config_result_array_t();
        }
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
            //cout <<"file<" << xml_ << "> parse failed:" << e_.what() <<"\n";
        }
        {
            scoped_lock_t lock(m_mutex);
            m_xml_cache_store[xml_] = ret;
        }
        return 0;
    }

    config_result_array_t* alloc_xml_cache(const string& xml_)
    {
        scoped_lock_t lock(m_mutex);
        config_result_array_t* ret = NULL;

        map<string, config_result_array_t*>::iterator it = m_xml_cache_store.find(xml_);
        if (it != m_xml_cache_store.end())
        {
            ret = it->second;
            m_xml_cache_store.erase(it);
        }
        return ret;
    }
    void clear()
    {
        scoped_lock_t lock(m_mutex);
        map<string, config_result_array_t*>::iterator it = m_xml_cache_store.begin();
        for (; it != m_xml_cache_store.end(); ++it)
        {
            cout << "not use:" << it->first << "\n";
            delete it->second;
        }
        m_xml_cache_store.clear();
    }
private:
    mutex_t                                 m_mutex;
    map<string, config_result_array_t*>     m_xml_cache_store;
};
#endif //XML_CONFIG_CACHE_H_

