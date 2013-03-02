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
#include "rpc/ffrpc.h"
#include "base/time_tool.h"
#include "net/msg_handler_i.h"
#include "base/os_tool.h"

#include "rapidjson/document.h"     // rapidjson's DOM-style API                                                                                             
#include "rapidjson/prettywriter.h" // for stringify JSON
#include "rapidjson/filestream.h"   // wrapper of C stream for prettywriter as output
#include "rapidjson/stringbuffer.h"

typedef runtime_error       msg_exception_t;
typedef rapidjson::Document json_dom_t;
typedef rapidjson::Value    json_value_t;

namespace ff
{

class event_log_t : public msg_i
{
public:
    event_log_t():msg_i("event_log_t"),m_db_name("default"){}
    event_log_t(const string& table_name_, const string& field_name_):
        msg_i("event_log_t"),
        m_db_name("default"),
        m_table_name(table_name_),
        m_field_names(field_name_)
    {
    }
    event_log_t(const string& db_name_, const string& table_name_, const string& field_name_):
        msg_i("event_log_t"),
        m_db_name(db_name_),
        m_table_name(table_name_),
        m_field_names(field_name_)
    {
    }
    virtual ~event_log_t(){}
    virtual string encode()
    {
        return (init_encoder()<< m_db_name << m_table_name << m_field_names << m_values).get_buff();
    }
    virtual void decode(const string& src_buff_)
    {
        init_decoder(src_buff_) >> m_db_name >> m_table_name >> m_field_names >> m_values;
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
    template<typename ARG1, typename ARG2, typename ARG3, typename ARG4, typename ARG5, typename ARG6, typename ARG7>
    void def(const ARG1& arg1_, const ARG2& arg2_, const ARG3& arg3_, const ARG4& arg4_, const ARG5& arg5_, const ARG6& arg6_, const ARG7& arg7_)
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
        {
            ss << arg7_;
            ss >> ret;
            m_values.push_back(ret);
            ss.clear();ret.clear();
        }
    }
    template<typename ARG1, typename ARG2, typename ARG3, typename ARG4, typename ARG5, typename ARG6, typename ARG7, typename ARG8>
    void def(const ARG1& arg1_, const ARG2& arg2_, const ARG3& arg3_, const ARG4& arg4_, const ARG5& arg5_, const ARG6& arg6_, const ARG7& arg7_, const ARG8& arg8_)
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
        {
            ss << arg7_;
            ss >> ret;
            m_values.push_back(ret);
            ss.clear();ret.clear();
        }
        {
            ss << arg8_;
            ss >> ret;
            m_values.push_back(ret);
            ss.clear();ret.clear();
        }
    }
    template<typename ARG1, typename ARG2, typename ARG3, typename ARG4, typename ARG5, typename ARG6, typename ARG7, typename ARG8, typename ARG9>
    void def(const ARG1& arg1_, const ARG2& arg2_, const ARG3& arg3_, const ARG4& arg4_, const ARG5& arg5_, const ARG6& arg6_, const ARG7& arg7_, const ARG8& arg8_, const ARG9& arg9_)
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
        {
            ss << arg7_;
            ss >> ret;
            m_values.push_back(ret);
            ss.clear();ret.clear();
        }
        {
            ss << arg8_;
            ss >> ret;
            m_values.push_back(ret);
            ss.clear();ret.clear();
        }
        {
            ss << arg9_;
            ss >> ret;
            m_values.push_back(ret);
            ss.clear();ret.clear();
        }
    }
public:
    string              m_db_name;
    string              m_table_name;
    string              m_field_names;
    vector<string>      m_values;
};
struct event_queryt_t
{
    struct in_t: public msg_i
    {
        in_t():
            msg_i("event_queryt_t::in_t"),
            db_name("default")
        {}
        virtual string encode()
        {
            return (init_encoder() << str_time << db_name << sql).get_buff();
        }
        virtual void decode(const string& src_buff_)
        {
            init_decoder(src_buff_) >> str_time >> db_name >> sql;
        }
        string str_time;
        string db_name;
        string sql;
    };
    struct out_t: public msg_i
    {
        out_t():
            msg_i("event_queryt_t::out_t")
        {}
        virtual string encode()
        {
            return (init_encoder() << ret_data << col_names << err_msg).get_buff();
        }
        virtual void decode(const string& src_buff_)
        {
            init_decoder(src_buff_) >> ret_data >> col_names >> err_msg;
        }
        vector<vector<string> > ret_data;
        vector<string>          col_names;
        string                  err_msg;
    };
};
class ffcount_t
{
public:
    ffcount_t()
    {}
    virtual ~ffcount_t(){}
    int save_event(const event_log_t& event_log_)
    {
        set<string>& m_fields = m_table2fields[event_log_.m_table_name];
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
                n += snprintf(buff + n, sizeof(buff) - n, "'%s'", get_db().escape(event_log_.m_values[i]).c_str());
            }
            else
            {
                n += snprintf(buff + n, sizeof(buff) - n, ",'%s'", get_db().escape(event_log_.m_values[i]).c_str());
            }
        }
        n += snprintf(buff + n, sizeof(buff) - n, ")");
        //printf("buff=%s\n", buff);
        if (get_db().exe_sql(buff))
        {
            printf("exe sql failed[%s]\n", buff);
            return -1;
        }
        return 0;
    }

    ffdb_t& get_db() { return m_ffdb; } 
    void init_fields(const string& table_)
    {
        set<string>& m_fields = m_table2fields[table_];
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
    ffdb_t                              m_ffdb;
    map<string, set<string> >           m_table2fields;
};

class ffcount_service_t: public msg_handler_i
{
    struct table_info_t
    {
        table_info_t():
            need_create_flag(true),
            tq(NULL){}
        bool             need_create_flag;
        task_queue_t*    tq;
        ffcount_t        ffcount;
        set<string>      table_names;
    };
    typedef map<string, table_info_t> table_info_map_t;
public:
    ffcount_service_t():
        m_path("./"),
        m_index(0)
    {}
    int start(string config_ = "")
    {
        if (false == config_.empty()) m_path = config_;
        
        for (int i = 0; i < 8; ++i)
        {
            m_all_tq.push_back(new task_queue_t());
            m_thread.create_thread(task_binder_t::gen(&task_queue_t::run, m_all_tq[i]), 1);
        }
        
        uint64_t dest_tm = time_tool_t::next_month() - ::time(NULL) + 1;
        m_timer_service.once_timer(dest_tm*1000, task_binder_t::gen(&ffcount_service_t::create_new_dir, this));
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

    int connect_db(ffcount_t& ffcount, const string& db_name)
    {
        ffdb_t& ffdb = ffcount.get_db();
        time_t now   = ::time(NULL);
        tm    tm_val = *::localtime(&now);
        char buff[256];
        snprintf(buff, sizeof(buff), "%s/%d/", m_path.c_str(), tm_val.tm_year + 1900);
        ::mkdir(buff, 0777);
        snprintf(buff, sizeof(buff), "%s/%d/%d", m_path.c_str(), tm_val.tm_year + 1900, tm_val.tm_mon+1);
        ::mkdir(buff, 0777);
        snprintf(buff, sizeof(buff), "sqlite://%s/%d/%d/%s.db", m_path.c_str(), tm_val.tm_year + 1900, tm_val.tm_mon+1, db_name.c_str());
        printf("connect new db<%s>\n", buff);
        return ffdb.connect(buff);
    }
    int check_db_valid(table_info_t& info, const string& db_name, const string& table_name)
    {
        if (true == info.need_create_flag)
        {
            if (NULL == info.tq)
            {
                task_queue_t* p    = m_all_tq[m_index++ % m_all_tq.size()];
                info.tq = p;
            }
            
            if (connect_db(info.ffcount, db_name))
            {
                printf("connect failed\n");
                return -1;
            }
            info.need_create_flag = false;            
        }
        if (table_name.empty() || info.table_names.find(table_name) != info.table_names.end())
        {
            return 0;
        }
        char buff[256];
        snprintf(buff, sizeof(buff), "create table IF NOT EXISTS %s(autoid integer PRIMARY KEY autoincrement, logtime TIMESTAMP default (datetime('now', 'localtime')))",
                 table_name.c_str());
        if (info.ffcount.get_db().exe_sql(buff))
        {
            printf("create table failed[%s]\n", buff);
            return -1;
        }
        printf("sql<%s>\n", buff);
        info.table_names.insert(table_name);
        return 0;
    }
    int save_event(event_log_t& in_msg_, rpc_callcack_t<bool_ret_msg_t>& cb_)
    {
        bool_ret_msg_t ret;ret.value = true;
        cb_(ret);
        lock_guard_t lock(m_mutex);
        table_info_t& info = m_db_info[in_msg_.m_db_name];
        if (check_db_valid(info, in_msg_.m_db_name, in_msg_.m_table_name))
        {
            return -1;
        }
        info.tq->produce(task_binder_t::gen(&ffcount_t::save_event, &(info.ffcount), in_msg_));
        return 0;
    }
    int query(event_queryt_t::in_t& in_msg_, rpc_callcack_t<event_queryt_t::out_t>& cb_)
    {
        lock_guard_t lock(m_mutex);
        table_info_t& info = m_db_info[in_msg_.db_name];
        if (check_db_valid(info, in_msg_.db_name, ""))
        {
            event_queryt_t::out_t ret;
            cb_(ret);
            return -1;
        }
        info.tq->produce(task_binder_t::gen(&ffcount_service_t::query_impl, this, in_msg_, cb_, &(info.ffcount)));
        return 0;
    }
    int query_impl(event_queryt_t::in_t in_msg_, rpc_callcack_t<event_queryt_t::out_t> cb_, ffcount_t* ffcount_)
    {
        event_queryt_t::out_t ret;
        char buff[256];
        time_t now   = ::time(NULL);
        tm    tm_val = *::localtime(&now);
        snprintf(buff, sizeof(buff), "%d/%d", tm_val.tm_year + 1900, tm_val.tm_mon+1);
        if (in_msg_.str_time.empty() || in_msg_.str_time == buff)
        {
            if (ffcount_->get_db().exe_sql(in_msg_.sql, ret.ret_data, ret.col_names))
            {
                ret.err_msg = ffcount_->get_db().error_msg();
            }
            cb_(ret);
            return 0;
        }
        else
        {
            snprintf(buff, sizeof(buff), "sqlite://%s/%s/%s.db", m_path.c_str(), in_msg_.str_time.c_str(), in_msg_.db_name.c_str());
            ffdb_t ffdb;
            if (ffdb.connect(buff))
            {
                ret.err_msg = ffcount_->get_db().error_msg();
                cb_(ret);
                return -1;
            }
            if (ffdb.exe_sql(in_msg_.sql, ret.ret_data, ret.col_names))
            {
                ret.err_msg = ffcount_->get_db().error_msg();
            }
            cb_(ret);
            return 0;
        }
        
        return 0;
    }
    void create_new_dir()
    {
        uint64_t dest_tm = time_tool_t::next_month() - ::time(NULL) + 1;
        m_timer_service.once_timer(dest_tm*1000, task_binder_t::gen(&ffcount_service_t::create_new_dir, this));
        lock_guard_t lock(m_mutex);
        for (table_info_map_t::iterator it = m_db_info.begin(); it != m_db_info.end(); ++it)
        {
            if (it->second.tq)
                it->second.tq->produce(task_binder_t::gen(&ffcount_service_t::clear_ops, this, &(it->second)));
        }
    }
    void clear_ops(table_info_t* table_info_)
    {
        table_info_->need_create_flag = true; //! 下次会自动创建新table
    }
    
    virtual int handle_broken(socket_ptr_t sock_)
    {
        lock_guard_t lock(m_mutex);
        sock_->safe_delete();
        m_tmp_socket_cache.erase(sock_);
        return 0;
    }
    virtual int handle_msg(const message_t& msg_, socket_ptr_t sock_)
    {
        string arg = msg_.get_body();
        
        arg = strtool_t::replace(arg, "%20", " ");
        arg = strtool_t::replace(arg, "%2A", "*");
        arg = strtool_t::replace(arg, "%2B", "+");
        arg = strtool_t::replace(arg, "%2F", "/");
        arg = strtool_t::replace(arg, "%3F", "?");
        arg = strtool_t::replace(arg, "%25", "%");
        arg = strtool_t::replace(arg, "%23", "#");
        arg = strtool_t::replace(arg, "%26", "&");
        arg = strtool_t::replace(arg, "%3D", "=");
        printf("http query param<%s>, convert to<%s>\n", msg_.get_body().c_str(), arg.c_str());
        
        vector<string> parse_ret;
        if (arg == "/")//! 返回所有目录和文件
        {
            rapidjson::Document::AllocatorType allocator;
            rapidjson::StringBuffer            str_buff;
            json_value_t                       ret_json(rapidjson::kObjectType);

            {
                json_value_t tmp_val("", 0, allocator);
                ret_json.AddMember("err_msg", tmp_val, allocator);
            }
            vector<string> year_dir;
            vector<string> all_dir;
            os_tool_t::ls(m_path.c_str(), year_dir);
            for (size_t i = 0; i < year_dir.size(); ++i)
            {
                if (false == os_tool_t::is_dir(year_dir[i])) continue;
                vector<string> ret;
                os_tool_t::ls((m_path + year_dir[i]).c_str(), ret);
                for (size_t j = 0; j < ret.size(); ++j)
                {
                    all_dir.push_back(m_path + year_dir[i] + "/" + ret[j]);
                }
            }
            
            {
                json_value_t tmp_val(rapidjson::kArrayType);
                {
                    json_value_t v("date", 4, allocator);
                    tmp_val.PushBack(v, allocator);
                    json_value_t v2("dbname", 6, allocator);
                    tmp_val.PushBack(v2, allocator);
                }
                ret_json.AddMember("col_names", tmp_val, allocator);
            }
            {
                json_value_t data_array(rapidjson::kArrayType);
                for (size_t i = 0; i < all_dir.size(); ++i)
                {
                    if (false == os_tool_t::is_dir(all_dir[i])) continue;
                    json_value_t tmp_val(rapidjson::kArrayType);
                    vector<string> ret;
                    os_tool_t::ls((all_dir[i]).c_str(), ret);
                    for (size_t j = 0; j < ret.size(); ++j)
                    {
                        json_value_t tmv(all_dir[i].c_str(), all_dir[i].length(), allocator);
                        tmp_val.PushBack(tmv, allocator);
                        json_value_t v(ret[j].c_str(), ret[j].length(), allocator);
                        tmp_val.PushBack(v, allocator);
                        data_array.PushBack(tmp_val, allocator);
                    }
                }
                ret_json.AddMember("ret_data", data_array, allocator);
            }
            rapidjson::Writer<rapidjson::StringBuffer> writer(str_buff, &allocator);
            ret_json.Accept(writer);
            string output(str_buff.GetString(), str_buff.GetSize());
            sock_->async_send(output);return 0;
        }
        strtool_t::split(arg, parse_ret, "/");
        if (parse_ret.size() != 4)
        {
            sock_->async_send("/time/db_name/sql format needed");return 0;
        }
        string str_time = parse_ret[0] + "/" + parse_ret[1];
        string db_name = parse_ret[2];
        string sql = parse_ret[3];
        
        lock_guard_t lock(m_mutex);
        table_info_t& info = m_db_info[db_name];
        if (check_db_valid(info, db_name, ""))
        {
            sock_->async_send("db connect failed");return 0;
        }
        //! 记录该socket有效，由于是异步操作，有可能在此期间变成失效
        m_tmp_socket_cache.insert(sock_);
        info.tq->produce(task_binder_t::gen(&ffcount_service_t::http_query_impl, this, &(info.ffcount), sock_, str_time, db_name, sql));
        
        return 0;
    }
    int http_query_impl(ffcount_t* ffcount_, socket_ptr_t sock_, string str_time, string db_name, string sql)
    {
        char buff[256];
        string err_msg;
        vector<vector<string> > ret_data;
        vector<string> col_names;
        time_t now   = ::time(NULL);
        tm    tm_val = *::localtime(&now);
        snprintf(buff, sizeof(buff), "%d/%d", tm_val.tm_year + 1900, tm_val.tm_mon+1);
        if (str_time.empty() || str_time == buff)
        {
            if (ffcount_->get_db().exe_sql(sql, ret_data, col_names))
            {
                err_msg = ffcount_->get_db().error_msg();
            }
        }
        else
        {
            snprintf(buff, sizeof(buff), "sqlite://%s/%s/%s.db", m_path.c_str(), str_time.c_str(), db_name.c_str());
            ffdb_t ffdb;
            if (ffdb.connect(buff))
            {
                err_msg = ffcount_->get_db().error_msg();
            }
            else
            {
                if (ffdb.exe_sql(sql, ret_data, col_names))
                {
                    err_msg = ffcount_->get_db().error_msg();
                }
            }
        }
        rapidjson::Document::AllocatorType allocator;
        rapidjson::StringBuffer            str_buff;
        json_value_t                       ret_json(rapidjson::kObjectType);
        
        {
            json_value_t tmp_val(err_msg.c_str(), err_msg.length(), allocator);
            ret_json.AddMember("err_msg", tmp_val, allocator);
        }
        {
            json_value_t tmp_val(rapidjson::kArrayType);
            for (size_t i = 0; i < col_names.size(); ++i)
            {
                json_value_t v(col_names[i].c_str(), col_names[i].length(), allocator);
                tmp_val.PushBack(v, allocator);
            }
            ret_json.AddMember("col_names", tmp_val, allocator);
        }
        {
            json_value_t data_array(rapidjson::kArrayType);
            for (size_t i = 0; i < ret_data.size(); ++i)
            {
                json_value_t tmp_val(rapidjson::kArrayType);
                for (size_t j = 0; j < ret_data[i].size(); ++j)
                {
                    json_value_t v(ret_data[i][j].c_str(), ret_data[i][j].length(), allocator);
                    tmp_val.PushBack(v, allocator);
                }
                data_array.PushBack(tmp_val, allocator);
            }
            ret_json.AddMember("ret_data", data_array, allocator);
        }
        rapidjson::Writer<rapidjson::StringBuffer> writer(str_buff, &allocator);
        ret_json.Accept(writer);
        string output(str_buff.GetString(), str_buff.GetSize());
        lock_guard_t lock(m_mutex);
        if (m_tmp_socket_cache.erase(sock_))
        {
            sock_->async_send(output);
        }
        return 0;
    }
private:
    string                      m_path;
    int                         m_index;
    table_info_map_t            m_db_info;
    thread_t                    m_thread;
    vector<task_queue_t*>       m_all_tq;
    mutex_t                     m_mutex;
    timer_service_t             m_timer_service;
    set<socket_ptr_t>           m_tmp_socket_cache;
};




}

#endif
