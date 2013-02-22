#ifndef _FFCRUD_H_
#define _FFCRUD_H_

#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <set>
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
        m_table_name(table_name_),
        m_field_names(field_name_)
    {
    }
    virtual ~event_log_t(){}
    virtual string encode()
    {
        return (init_encoder()<< m_table_name << m_field_names << m_values).get_buff();
    }
    virtual void decode(const string& src_buff_)
    {
        init_decoder(src_buff_) >> m_table_name >> m_field_names >> m_values;
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
    string              m_table_name;
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
    {}
    virtual ~ffcount_t(){}
    int save_event(const event_log_t& event_log_)
    {
        if (m_fields.empty())
        {
            init_fields(event_log_.m_table_name);
        }
        char buff[1024];
        int n = snprintf(buff, sizeof(buff), "INSERT INTO %s (", event_log_.m_table_name.c_str());
        vector<string> str_vt;
        strtool_t::split(event_log_.m_field_names, str_vt, ",");
        for (size_t i = 0; i < str_vt.size(); ++i)
        {
            if (m_fields.find(str_vt[i]) == m_fields.end())
            {
                //! 增加字段
                m_fields.insert(str_vt[i]);
                string sql = string("ALTER TABLE ") + event_log_.m_table_name + " ADD " + str_vt[i] +" varchar(255) default ''";
                get_db().exe_sql(sql);
            }
        }
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
        if (get_db().exe_sql(buff))
        {
            printf("exe sql failed[%s]\n", buff);
            return -1;
        }
        return 0;
    }

    void dump(vector<vector<string> >& ret_data, vector<string>& col_names_)
    {
        for (size_t i = 0; i < col_names_.size(); ++i)
        {
            printf("    %s", col_names_[i].c_str());
        }
        printf("\n");
        for (size_t i = 0; i < ret_data.size(); ++i)
        {
            for (size_t j = 0; j < ret_data[i].size(); ++j)
            {
                printf(" %s", ret_data[i][j].c_str());
            }
            printf("\n");
        }
        ret_data.clear();
    }
    int query(const string& sql_)
    {
        vector<vector<string> > ret_data;
        vector<string> col_names;
        int ret = get_db().exe_sql(sql_, ret_data, col_names);
        dump(ret_data, col_names);
        return ret;
    }
    ffdb_t& get_db() { return m_ffdb; } 
    void init_fields(const string& table_)
    {
        string sql = string("select * from ") + table_ + " limit 1";
        vector<vector<string> > ret_data;
        vector<string> col_names;
        get_db().exe_sql(sql, ret_data, col_names);
        for (size_t i = 0; i < col_names.size(); ++i)
        {
            m_fields.insert(col_names[i]);
        }
    }
protected:
    ffdb_t              m_ffdb;
    set<string>         m_fields;
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
        m_path("./"),
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
        }
        
        m_thread.join();
        for (unsigned int i = 0; i < m_all_tq.size(); ++i)
        {
            delete m_all_tq[i];
        }
        m_all_tq.clear();
        return 0;
    }

    int save_event(event_log_t& in_msg_, rpc_callcack_t<event_ret_t>& cb_)
    {
        event_ret_t ret;
        cb_(ret);
        table_info_t& info = m_db_info[in_msg_.m_table_name];
        if (NULL == info.tq)
        {
            task_queue_t* p    = m_all_tq[m_index++ % m_all_tq.size()];
            info.tq = p;
            time_t now   = ::time(NULL);
            tm    tm_val = *::localtime(&now);
            char buff[256];
            snprintf(buff, sizeof(buff), "%s/%d/", m_path.c_str(), tm_val.tm_year + 1900);
            ::mkdir(buff, 0777);
            snprintf(buff, sizeof(buff), "%s/%d/%d", m_path.c_str(), tm_val.tm_year + 1900, tm_val.tm_mon+1);
            ::mkdir(buff, 0777);
            snprintf(buff, sizeof(buff), "sqlite://%s/%d/%d/%s.db", m_path.c_str(), tm_val.tm_year + 1900, tm_val.tm_mon+1, in_msg_.m_table_name.c_str());
            if (info.ffcount.get_db().connect(buff))
            {
                printf("connect failed[%s]\n", buff);
                return -1;
            }
            
            snprintf(buff, sizeof(buff), "create table IF NOT EXISTS %s(auto_id integer PRIMARY KEY autoincrement, logtime TIMESTAMP default (datetime('now', 'localtime')))",
                     in_msg_.m_table_name.c_str());
            if (info.ffcount.get_db().exe_sql(buff))
            {
                printf("create table failed[%s]\n", buff);
                return -1;
            }
            info.ffcount.query("select * from dumy");
        }
        info.tq->produce(task_binder_t::gen(&ffcount_t::save_event, &(info.ffcount), in_msg_));
        return 0;
    }
private:
    string                      m_path;
    int                         m_index;
    table_info_map_t            m_db_info;
    thread_t                    m_thread;
    vector<task_queue_t*>       m_all_tq;
};



}

#endif
