
#ifndef _FF_SQLITE_OPS_H_
#define _FF_SQLITE_OPS_H_

#include "db/db_ops.h"
#include "db/sqlite3.h"

#include <vector>
using namespace std;

namespace ff
{

class sqlite_ops_t : public db_ops_i
{
public:
    struct callback_info_t
    {
        callback_info_t(sqlite_ops_t* p = NULL):
            obj(p),
            callback(NULL)
        {}
        sqlite_ops_t* obj;
        db_each_row_callback_i* callback;
        vector<long>            length_buff;
    };
public:
    sqlite_ops_t();
    virtual ~sqlite_ops_t();

    virtual int  connect(const string& args_);
    virtual bool is_connected();
    virtual int  exe_sql(const string& sql_, db_each_row_callback_i* cb_);
    virtual void close();
    virtual int  affect_rows() { return m_affect_rows_num; }
    virtual const char*  error_msg();

    void inc_affect_row_num()  { ++ m_affect_rows_num; }
    
    virtual string escape(const string& src_);
    
    virtual void begin_transaction();
    virtual void commit_transaction();
    virtual void rollback_transaction();
private:
    void clear_env();
private:
    sqlite3* m_sqlite;
    bool     m_connected;
    string   m_error;
    int      m_affect_rows_num;
    callback_info_t m_callback_info;
};
}

#endif
