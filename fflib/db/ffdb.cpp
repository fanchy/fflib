
#include "db/ffdb.h"
#include "db/sqlite_ops.h"
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
    return m_db_ops->connect(args_);
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