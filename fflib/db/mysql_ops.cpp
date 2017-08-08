#ifdef _WIN32
#include <winsock2.h>
#endif

#include "db/mysql_ops.h"
#include "base/strtool.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
using namespace std;
using namespace ff;

mutex_t mysql_ops_t::g_mutex;

mysql_ops_t::mysql_ops_t():
    m_connected(false),
    m_affected_rows_num(0)
{
}
mysql_ops_t::~mysql_ops_t()
{
    close();
}
void mysql_ops_t::clear_env()
{
    m_affected_rows_num = 0; 
    m_error.clear();
}

int mysql_ops_t::connect(const string& args_)
{
    lock_guard_t lock(g_mutex);
    clear_env();
    close();
    m_connected = false;

    m_host_arg = args_;
    vector<string> str_vt;
    strtool_t::split(m_host_arg, str_vt, "/");//! 127.0.0.1:3306/user/passwd/db

    //! set mysql connection charset as utf8
    if (str_vt.size() < 3)
    {
        return -1;
    }
    vector<string> host_vt;
    strtool_t::split(str_vt[0], host_vt, ":");
    string host = host_vt[0];
    string port = "3306";
    if (host_vt.size() >= 2)
    {
        port = host_vt[1];
    }
    string user   = str_vt[1];
    string passwd = str_vt[2];
    string db     = "";
    if (str_vt.size() >= 4)
    {
        db = str_vt[3];
    }
    
    if(!mysql_init(&m_mysql))
    {
        m_error = mysql_error(&m_mysql);
        return -1;
    }
    ::mysql_options(&m_mysql, MYSQL_SET_CHARSET_NAME, "utf8");
    if (!::mysql_real_connect(&m_mysql, host.c_str(), user.c_str(), passwd.c_str(), db.c_str(), atoi(port.c_str()), NULL, 0))
    {
        m_error = mysql_error(&m_mysql);
        return -1;
    }
    char value = 1;
    ::mysql_options(&m_mysql, MYSQL_OPT_RECONNECT, (char *)&value);

    m_connected = true;
    return 0;
}

bool mysql_ops_t::is_connected()
{
    return m_connected;
}

int  mysql_ops_t::exe_sql(const string& sql_, db_each_row_callback_i* cb_)
{
    clear_env();

    if (::mysql_query(&m_mysql, sql_.c_str()))
    {
        bool err = true;
        if(CR_SERVER_GONE_ERROR == mysql_errno(&m_mysql) || CR_SERVER_LOST == mysql_errno(&m_mysql))
        {
            //! reconnect and try it again
            ping();
            if (0 == ::mysql_query(&m_mysql, sql_.c_str()))
            {
                err = false;
            }
        }
        if (err)
        {
            m_error = ::mysql_error(&m_mysql);
            return -1;
        }
    }
    m_affected_rows_num = (int)::mysql_affected_rows(&m_mysql);
    
    if (cb_ == NULL)
    {
        return 0;
    }
    MYSQL_RES* res = ::mysql_store_result(&m_mysql);
    
    if (res)
    {
        MYSQL_FIELD* column_infos = ::mysql_fetch_fields(res);
        int column_num = ::mysql_num_fields(res);
        vector<char*> vt_col_name;
        for (int c = 0; c < column_num; ++ c)
        {
            vt_col_name.push_back(column_infos[c].name);
        }
        char** pcol_name = &(vt_col_name.front());
        

        int num_row = ::mysql_num_rows(res);
        for (int i= 0; i < num_row; ++i)
        {
            MYSQL_ROW row = ::mysql_fetch_row(res);
            unsigned long* plen = ::mysql_fetch_lengths(res);
            cb_->callback(column_num, row, (char**)pcol_name, (long*)plen);
        }
        ::mysql_free_result(res);
        res = NULL;
    }
    return 0;
}
void mysql_ops_t::close()
{
    if (m_connected)
    {
        mysql_close(&m_mysql);
        m_connected = false;
    }
}
const char*  mysql_ops_t::error_msg()
{
    return m_error.c_str();
}


void mysql_ops_t::begin_transaction()
{
    static string sql = "START TRANSACTION;";
    exe_sql(sql, NULL);
}

void mysql_ops_t::commit_transaction()
{
    static string sql = "COMMIT;";
    exe_sql(sql, NULL);
}

void mysql_ops_t::rollback_transaction()
{
    static string sql = "ROLLBACK;";
    exe_sql(sql, NULL);
}

int mysql_ops_t::ping()
{
    return ::mysql_ping(&m_mysql);
}

