
#include <stdio.h>
#include "db/ffdb.h"
#include "db/sqlite_ops.h"
#include "base/strtool.h"
using namespace ff;

ffdb_t::ffdb_t():
    m_db_ops(NULL)
{
    
}

ffdb_t::~ffdb_t()
{
    close();
}

int  ffdb_t::connect(const string& args_)
{
    close();
    m_db_ops = new sqlite_ops_t();
    vector<string> str_vt;
    strtool_t::split(args_, str_vt, "://");
    if (str_vt.size() == 2 && str_vt[0] == "sqlite")
    {
        return m_db_ops->connect(str_vt[1]);
    }
    return -1;
}

bool ffdb_t::is_connected()
{
    return m_db_ops->is_connected();
}

void ffdb_t::close()
{
    if (m_db_ops)
    {
        m_db_ops->close();
        delete m_db_ops;
        m_db_ops = NULL;
    }
}
int  ffdb_t::affect_rows()
{
    return m_db_ops->affect_rows();
}
const char*  ffdb_t::error_msg()
{
    return m_db_ops->error_msg();
}


int  ffdb_t::exe_sql(const string& sql_, db_each_row_callback_i* cb_)
{
    return m_db_ops->exe_sql(sql_, cb_);
}
int  ffdb_t::exe_sql(const string& sql_, vector<vector<string> >& ret_data_)
{
    each_row_common_cb_t cb(&ret_data_, NULL);
    return m_db_ops->exe_sql(sql_, &cb);
}
int  ffdb_t::exe_sql(const string& sql_, vector<vector<string> >& ret_data_, vector<string>& col_names_)
{
    each_row_common_cb_t cb(&ret_data_, &col_names_);
    return m_db_ops->exe_sql(sql_, &cb);
}

void ffdb_t::dump(vector<vector<string> >& ret_data, vector<string>& col_names_)
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

string ffdb_t::escape(const string& src_)
{
    return m_db_ops->escape(src_);
}
