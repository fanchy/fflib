
#include "db/sqlite_ops.h"
#include <string.h>

using namespace ff;

sqlite_ops_t::sqlite_ops_t():
    m_sqlite(NULL),
    m_connected(false),
    m_affect_rows_num(0)
{
    m_callback_info.obj = this;
}
sqlite_ops_t::~sqlite_ops_t()
{
    close();
}
void sqlite_ops_t::clear_env()
{
    m_affect_rows_num = 0; 
    m_error.clear();
}

int sqlite_ops_t::connect(const string& args_)
{
    clear_env();
    close();
    if (SQLITE_OK != ::sqlite3_open(args_.c_str(), &m_sqlite))
    {
        m_connected = false;
        m_error = sqlite3_errmsg(m_sqlite);
        return -1;
    }
    m_connected = true;
    return 0;
}

bool sqlite_ops_t::is_connected()
{
    return m_connected;
}

static int default_callback(void* p_, int col_num_, char** col_datas_, char** col_names_)
{
    sqlite_ops_t::callback_info_t* info = (sqlite_ops_t::callback_info_t*)(p_);
    info->obj->inc_affect_row_num();
    if (info->callback)
    {
        for (int i = 0; i < col_num_; ++i)
        {
            info->length_buff.push_back(::strlen(col_datas_[i]));
        }
        long* ptr_length = col_num_ == 0? NULL: &(info->length_buff[0]);
        info->callback->callback(col_num_, col_datas_, col_names_, ptr_length);
        info->length_buff.clear();
    }
    return 0;
}

int  sqlite_ops_t::exe_sql(const string& sql_, db_each_row_callback_i* cb_)
{
    char* msg = NULL;
    clear_env();
    m_callback_info.callback = cb_;
    if (::sqlite3_exec(m_sqlite, sql_.c_str(), &default_callback, &m_callback_info, &msg))
    {
        if (msg)
        {
            m_error = msg;
            ::sqlite3_free(msg);
        }
        return -1;
    }
    return 0;
}
void sqlite_ops_t::close()
{
    if (m_sqlite)
    {
        ::sqlite3_close(m_sqlite);
        m_sqlite = NULL;
    }
}
const char*  sqlite_ops_t::error_msg()
{
    return m_error.c_str();
}
