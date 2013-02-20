#ifndef _FFCRUD_H_
#define _FFCRUD_H_

#include <string>
#include <sstream>
#include <vector>
#include <map>
using namespace std;

#include <string.h>
#include <stdio.h>

#include "base/strtool.h"
#include "db/ffdb.h"

namespace ff
{

class event_log_t
{
public:
    event_log_t(const string& table_name_, const string& field_name_):
        m_table_names(table_name_)
    {
        strtool_t::split(field_name_, m_field_names, ",");
    }
    virtual ~event_log_t(){}
    template<typename ARG1>
    void def(const ARG1& arg1_)
    {
        stringstream ss;
        string ret;
        {
            ss << arg1_;
            ss >> ret;
            m_values.push_back(ret);
            ret.clear();
        }
    }
    template<typename ARG1, typename ARG2>
    void def(const ARG1& arg1_, const ARG2& arg2_)
    {
        stringstream ss;
        string ret;
        {
            ss << arg1_;
            ss >> ret;
            m_values.push_back(ret);
            ss.clear();
            ret.clear();
        }
        {
            ss << arg2_;
            ss >> ret;
            m_values.push_back(ret);
            ret.clear();
        }
    }
    template<typename ARG1, typename ARG2, typename ARG3>
    void def(const ARG1& arg1_, const ARG2& arg2_, const ARG3& arg3_)
    {
        stringstream ss;
        string ret;
        {
            ss << arg1_;
            ss >> ret;
            m_values.push_back(ret);
            ret.clear();
        }
        {
            ss << arg2_;
            ss >> ret;
            m_values.push_back(ret);
            ret.clear();
        }
        {
            ss << arg3_;
            ss >> ret;
            m_values.push_back(ret);
            ret.clear();
        }
    }
    template<typename ARG1, typename ARG2, typename ARG3, typename ARG4>
    void def(const ARG1& arg1_, const ARG2& arg2_, const ARG3& arg3_, const ARG4& arg4_)
    {
        stringstream ss;
        string ret;
        {
            ss << arg1_;
            ss >> ret;
            m_values.push_back(ret);
            ret.clear();
        }
        {
            ss << arg2_;
            ss >> ret;
            m_values.push_back(ret);
            ret.clear();
        }
        {
            ss << arg3_;
            ss >> ret;
            m_values.push_back(ret);
            ret.clear();
        }
        {
            ss << arg4_;
            ss >> ret;
            m_values.push_back(ret);
            ret.clear();
        }
    }
    template<typename ARG1, typename ARG2, typename ARG3, typename ARG4, typename ARG5>
    void def(const ARG1& arg1_, const ARG2& arg2_, const ARG3& arg3_, const ARG4& arg4_, const ARG5& arg5_)
    {
        stringstream ss;
        string ret;
        {
            ss << arg1_;
            ss >> ret;
            m_values.push_back(ret);
            ret.clear();
        }
        {
            ss << arg2_;
            ss >> ret;
            m_values.push_back(ret);
            ret.clear();
        }
        {
            ss << arg3_;
            ss >> ret;
            m_values.push_back(ret);
            ret.clear();
        }
        {
            ss << arg4_;
            ss >> ret;
            m_values.push_back(ret);
            ret.clear();
        }
        {
            ss << arg5_;
            ss >> ret;
            m_values.push_back(ret);
            ret.clear();
        }
    }
    template<typename ARG1, typename ARG2, typename ARG3, typename ARG4, typename ARG5, typename ARG6>
    void def(const ARG1& arg1_, const ARG2& arg2_, const ARG3& arg3_, const ARG4& arg4_, const ARG5& arg5_, const ARG6& arg6_)
    {
        stringstream ss;
        string ret;
        {
            ss << arg1_;
            ss >> ret;
            m_values.push_back(ret);
            ret.clear();
        }
        {
            ss << arg2_;
            ss >> ret;
            m_values.push_back(ret);
            ret.clear();
        }
        {
            ss << arg3_;
            ss >> ret;
            m_values.push_back(ret);
            ret.clear();
        }
        {
            ss << arg4_;
            ss >> ret;
            m_values.push_back(ret);
            ret.clear();
        }
        {
            ss << arg5_;
            ss >> ret;
            m_values.push_back(ret);
            ret.clear();
        }
        {
            ss << arg6_;
            ss >> ret;
            m_values.push_back(ret);
            ret.clear();
        }
    }

public:
    string              m_table_names;
    vector<string>      m_field_names;
    vector<string>      m_values;
};


class ffcount_t
{
public:
    ffcount_t()
    {
        get_db().connect("sqlite://./test.db");
        if (get_db().exe_sql("CREATE TABLE  IF NOT EXISTS dumy (A int, B varchar(200))"))
        {
            printf("exe error:%s\n", get_db().error_msg());
        }
    }
    virtual ~ffcount_t(){}
    int add_event(const event_log_t& event_log_)
    {
        char buff[1024];
        int n = snprintf(buff, sizeof(buff), "INSERT INTO %s (", event_log_.m_table_names.c_str());
        for (size_t i = 0; i < event_log_.m_field_names.size(); ++i)
        {
            if (i == 0)
            {
                n += snprintf(buff + n, sizeof(buff) - n, "%s", event_log_.m_field_names[i].c_str());
            }
            else
            {
                n += snprintf(buff + n, sizeof(buff) - n, ",%s", event_log_.m_field_names[i].c_str());
            }
        }
        n += snprintf(buff + n, sizeof(buff) - n, ") VALUES (");
        for (size_t i = 0; i < event_log_.m_values.size(); ++i)
        {
            if (i == 0)
            {
                n += snprintf(buff + n, sizeof(buff) - n, "'%s'", event_log_.m_values[i].c_str());
            }
            else
            {
                n += snprintf(buff + n, sizeof(buff) - n, ",'%s'", event_log_.m_values[i].c_str());
            }
        }
        n += snprintf(buff + n, sizeof(buff) - n, ")");
        //printf("buff=%s\n", buff);return 0;
        return get_db().exe_sql(buff);
    }

    void dump(vector<vector<string> >& ret_data)
    {
        for (size_t i = 0; i < ret_data.size(); ++i)
        {
            printf("row[%u] begin======= ", i);
            for (size_t j = 0; j < ret_data[i].size(); ++j)
            {
                printf(" %s", ret_data[i][j].c_str());
            }
            printf(" =======row[%u] end\n", i);
        }
        ret_data.clear();
    }
    int query(const string& sql_)
    {
        vector<vector<string> > ret_data;
        int ret = get_db().exe_sql(sql_, ret_data);
        dump(ret_data);
        return ret;
    }
    ffdb_t& get_db() { return m_ffdb; } 

protected:
    ffdb_t     m_ffdb;
};

}

#endif
