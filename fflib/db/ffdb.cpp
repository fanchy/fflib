#ifdef _WIN32
#include <winsock2.h>
#endif

#include <stdio.h>
#include "db/ffdb.h"
#include "base/strtool.h"
#include "db/sqlite_ops.h"
#include "db/mysql_ops.h"
using namespace std;
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
    vector<string> str_vt;
    strtool_t::split(args_, str_vt, "://");
    if (str_vt.size() == 2)
    {
        if (str_vt[0] == "sqlite")
        {
            #if FF_ENABLE_SQLITE
                m_db_ops = new sqlite_ops_t();
                return m_db_ops->connect(str_vt[1]);
            #endif
        }
        else
        {
            m_db_ops = new mysql_ops_t();
            return m_db_ops->connect(str_vt[1]);
        }
    }
    return -1;
}

bool ffdb_t::is_connected()
{
    return m_db_ops && m_db_ops->is_connected();
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
int  ffdb_t::affected_rows()
{
    if (m_db_ops)
        return m_db_ops->affected_rows();
    return 0;
}
const char*  ffdb_t::error_msg()
{
    if (m_db_ops)
        return m_db_ops->error_msg();
    return "none connection";
}


int  ffdb_t::exe_sql(const string& sql_, db_each_row_callback_i* cb_)
{
    if (m_db_ops)
        return m_db_ops->exe_sql(sql_, cb_);
    return -1;
}
int  ffdb_t::exe_sql(const string& sql_, vector<vector<string> >& ret_data_)
{
    if (m_db_ops)
    {
        each_row_common_cb_t cb(&ret_data_, NULL);
        return m_db_ops->exe_sql(sql_, &cb);
    }
    return -1;
}
int  ffdb_t::exe_sql(const string& sql_, vector<vector<string> >& ret_data_, vector<string>& col_names_)
{
    if (m_db_ops)
    {
        each_row_common_cb_t cb(&ret_data_, &col_names_);
        return m_db_ops->exe_sql(sql_, &cb);
    }
    return -1;
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

string ffdb_t::escape(const string& src_)
{
    string ret;
    //ret.resize(src_.size()*2 + 2);
    char* msg = new char[src_.size()*2 + 2];
    //size_t len = escape_string((char*)ret.data(), ret.size(), src_.c_str(), src_.size());
    size_t len = escape_string(msg, src_.size()*2 + 2, src_.c_str(), src_.size());
    //ret.erase(ret.begin() + len);
    ret.append(msg, len);
    delete[] msg;
    return ret;
}
int ffdb_t::ping()
{
    if (m_db_ops)
        return m_db_ops->ping();
    return -1;
}
