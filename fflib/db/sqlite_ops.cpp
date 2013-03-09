
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

static size_t escape_string(char *to, size_t to_length, const char *from, size_t length)
{
  const char *to_start= to;
  const char *end, *to_end=to_start + (to_length ? to_length-1 : 2*length);
  bool overflow= false;

  for (end= from + length; from < end; from++)
  {
    char escape= 0;
    switch (*from) {
    case 0:				/* Must be escaped for 'mysql' */
      escape= '0';
      break;
    case '\n':				/* Must be escaped for logs */
      escape= 'n';
      break;
    case '\r':
      escape= 'r';
      break;
    case '\\':
      escape= '\\';
      break;
    case '\'':
      escape= '\'';
      break;
    case '"':				/* Better safe than sorry */
      escape= '"';
      break;
    case '\032':			/* This gives problems on Win32 */
      escape= 'Z';
      break;
    }
    if (escape)
    {
      if (to + 2 > to_end)
      {
        overflow= true;
        break;
      }
      if (escape == '\'')
          *to++= '\'';
      else
          *to++= '\\';
      *to++= escape;
    }
    else
    {
      if (to + 1 > to_end)
      {
        overflow= true;
        break;
      }
      *to++= *from;
    }
  }
  *to= 0;
  return overflow ? (size_t) -1 : (size_t) (to - to_start);
}

string sqlite_ops_t::escape(const string& src_)
{
    string ret;
    ret.resize(src_.size()*2 + 2);
    size_t len = escape_string((char*)ret.data(), ret.size(), src_.c_str(), src_.size());
    ret.erase(ret.begin() + len);
    return ret;
}

void sqlite_ops_t::begin_transaction()
{
    exe_sql("begin transaction", NULL);
}

void sqlite_ops_t::commit_transaction()
{
    exe_sql("commit transaction", NULL);
}

void sqlite_ops_t::rollback_transaction()
{
    exe_sql("rollback transaction", NULL);
}
