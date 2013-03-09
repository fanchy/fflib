#ifndef _FF_DB_OPS_H_
#define _FF_DB_OPS_H_

#include <string>
using namespace std;

namespace ff
{

class db_each_row_callback_i
{
public:
    virtual ~db_each_row_callback_i() {}
    virtual void callback(int col_num_, char** col_datas_, char** col_names_, long* col_length_) = 0;
};

class db_ops_i
{
public:
    virtual ~db_ops_i(){}

    virtual int  connect(const string& args_) = 0;
    virtual bool is_connected() = 0;
    virtual int  exe_sql(const string& sql_, db_each_row_callback_i* cb_) = 0;
    virtual void close() = 0;
    virtual int  affect_rows() = 0;
    virtual const char*  error_msg() = 0;
    virtual string escape(const string& src_) = 0;
    
    virtual void begin_transaction() = 0;
    virtual void commit_transaction() = 0;
    virtual void rollback_transaction() = 0;
};

}
#endif



