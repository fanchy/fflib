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
#include "net/codec.h"
#include "base/task_queue_impl.h"
#include "rpc/msg_bus.h"

namespace ff
{

class event_log_t : public msg_i
{
public:
    event_log_t():msg_i("event_log_t"){}
    event_log_t(const string& table_name_, const string& field_name_):
        msg_i("event_log_t"),
        m_table_names(table_name_),
        m_field_names(field_name_)
    {
    }
    virtual ~event_log_t(){}
    virtual string encode()
    {
        return (init_encoder()<< m_table_names << m_field_names << m_values).get_buff();
    }
    virtual void decode(const string& src_buff_)
    {
        init_decoder(src_buff_) >> m_table_names >> m_field_names >> m_values;
    }
    template<typename ARG1>
    void def(const ARG1& arg1_)
    {
        stringstream ss;
        string ret;
        {
            ss << arg1_;
            ss >> ret;
            m_values.push_back(ret);
            ss.clear();ret.clear();
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
            ss.clear();
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
            ss.clear();
            ret.clear();
        }
        {
            ss << arg2_;
            ss >> ret;
            m_values.push_back(ret);
            ss.clear();ret.clear();
        }
        {
            ss << arg3_;
            ss >> ret;
            m_values.push_back(ret);
            ss.clear();ret.clear();
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
            ss.clear();ret.clear();
        }
        {
            ss << arg2_;
            ss >> ret;
            m_values.push_back(ret);
            ss.clear();ret.clear();
        }
        {
            ss << arg3_;
            ss >> ret;
            m_values.push_back(ret);
            ss.clear();ret.clear();
        }
        {
            ss << arg4_;
            ss >> ret;
            m_values.push_back(ret);
            ss.clear();ret.clear();
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
            ss.clear();ret.clear();
        }
        {
            ss << arg2_;
            ss >> ret;
            m_values.push_back(ret);
            ss.clear();ret.clear();
        }
        {
            ss << arg3_;
            ss >> ret;
            m_values.push_back(ret);
            ss.clear();ret.clear();
        }
        {
            ss << arg4_;
            ss >> ret;
            m_values.push_back(ret);
            ss.clear();ret.clear();
        }
        {
            ss << arg5_;
            ss >> ret;
            m_values.push_back(ret);
            ss.clear();ret.clear();
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
            ss.clear();ret.clear();
        }
        {
            ss << arg2_;
            ss >> ret;
            m_values.push_back(ret);
            ss.clear();ret.clear();
        }
        {
            ss << arg3_;
            ss >> ret;
            m_values.push_back(ret);
            ss.clear();ret.clear();
        }
        {
            ss << arg4_;
            ss >> ret;
            m_values.push_back(ret);
            ss.clear();ret.clear();
        }
        {
            ss << arg5_;
            ss >> ret;
            m_values.push_back(ret);
            ss.clear();ret.clear();
        }
        {
            ss << arg6_;
            ss >> ret;
            m_values.push_back(ret);
            ss.clear();ret.clear();
        }
    }

public:
    string              m_table_names;
    string              m_field_names;
    vector<string>      m_values;
};
struct event_ret_t: public msg_i
{
    event_ret_t():
        msg_i("event_ret_t"),
        value(true)
    {}
    virtual string encode()
    {
        return (init_encoder() << value).get_buff();
    }
    virtual void decode(const string& src_buff_)
    {
        init_decoder(src_buff_) >> value;
    }
    bool value;
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
    int save_event(const event_log_t& event_log_)
    {
        char buff[1024];
        int n = snprintf(buff, sizeof(buff), "INSERT INTO %s (", event_log_.m_table_names.c_str());
        vector<string> str_vt;
        strtool_t::split(event_log_.m_field_names, str_vt, ",");
        for (size_t i = 0; i < str_vt.size(); ++i)
        {
            if (i == 0)
            {
                n += snprintf(buff + n, sizeof(buff) - n, "%s", str_vt[i].c_str());
            }
            else
            {
                n += snprintf(buff + n, sizeof(buff) - n, ",%s", str_vt[i].c_str());
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

class ffcount_service_t
{
    struct table_info_t
    {
        table_info_t():
            tq(NULL){}
        task_queue_t*    tq;
        ffcount_t        ffcount;
    };
    typedef map<string, table_info_t> table_info_map_t;
public:
    ffcount_service_t():
        m_index(0)
    {}
    int start(string config_ = "")
    {
        for (int i = 0; i < 8; ++i)
        {
            m_all_tq.push_back(new task_queue_t());
            m_thread.create_thread(task_binder_t::gen(&task_queue_t::run, m_all_tq[i]), 1);
        }
        return 0;
    }
    int stop()
    {
        for (unsigned int i = 0; i < m_all_tq.size(); ++i)
        {
            m_all_tq[i]->close();
            delete m_all_tq[i];
        }
        m_all_tq.clear();
        return 0;
    }

    void save_event(event_log_t& in_msg_, rpc_callcack_t<event_ret_t>& cb_)
    {
        event_ret_t ret;
        cb_(ret);
        table_info_map_t::iterator it = m_db_info.find(in_msg_.m_table_names);
        if (it == m_db_info.end())
        {
            task_queue_t* p    = m_all_tq[m_index++ % m_all_tq.size()];
            table_info_t& info = m_db_info[in_msg_.m_table_names];
            info.tq = p;
            info.ffcount.get_db().connect("sqlite://./test.db");
            p->produce(task_binder_t::gen(&ffcount_t::save_event, &(it->second.ffcount), in_msg_));
        }
        else
        {
            it->second.tq->produce(task_binder_t::gen(&ffcount_t::save_event, &(it->second.ffcount), in_msg_));
        }
    }
private:
    int                         m_index;
    table_info_map_t            m_db_info;
    thread_t                    m_thread;
    vector<task_queue_t*>       m_all_tq;
};



}

#endif
