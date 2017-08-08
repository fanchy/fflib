#ifndef _MYSQL_OPS_H_
#define _MYSQL_OPS_H_

extern "C"{
#include <mysql/mysql.h>
#include <mysql/errmsg.h>
}

#include <string>


#include "db/db_ops.h"
#include "base/lock.h"

namespace ff
{

class mysql_ops_t : public db_ops_i
{
    static mutex_t g_mutex;

public:
    mysql_ops_t();
    virtual ~mysql_ops_t();

    virtual int  connect(const std::string& args_);
    virtual bool is_connected();
    virtual int  exe_sql(const std::string& sql_, db_each_row_callback_i* cb_);
    virtual void close();
    virtual int  affected_rows() { return m_affected_rows_num; }
    virtual const char*  error_msg();
    
    virtual void begin_transaction();
    virtual void commit_transaction();
    virtual void rollback_transaction();
    virtual int  ping();
    
private:
    void clear_env();
private:
    std::string   m_host_arg;
    MYSQL    m_mysql;
    bool     m_connected;
    std::string   m_error;
    int      m_affected_rows_num;

};
}
#endif

