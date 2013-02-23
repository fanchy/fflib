
//! 封装db的操作
#ifndef _FFDB_H_
#define _FFDB_H_

#include <string>
#include <vector>
using namespace std;

#include "db/db_ops.h"

namespace ff
{

class each_row_common_cb_t: public db_each_row_callback_i
{
public:
    each_row_common_cb_t(vector<vector<string> >* data_, vector<string>* col_name_):
        m_all_data(data_),
        m_col_name(col_name_)
    {}
    virtual void callback(int col_num_, char** col_datas_, char** col_names_, long* col_length_)
    {
        (*m_all_data).push_back(vector<string>());
        vector<string>& data = (*m_all_data)[(*m_all_data).size() - 1];
        data.resize(col_num_);
        for (int i = 0; i < col_num_; ++i)
        {
            if (col_datas_[i])
            {
                data[i].assign(col_datas_[i], col_length_[i]);
            }
        }
        if (m_col_name && (*m_col_name).empty())
        {
            (*m_col_name).resize(col_num_);
            for (int i = 0; i < col_num_; ++i)
            {
                (*m_col_name)[i].assign(col_names_[i]);
            }
        }
    }
private:
    vector<vector<string> >* m_all_data;
    vector<string>*          m_col_name;
};

class ffdb_t
{
public:
    ffdb_t();
    ~ffdb_t();
    
    string escape(const string& src_);
    static void dump(vector<vector<string> >& ret_data, vector<string>& col_names_);

    int  connect(const string& args_);
    bool is_connected();
    void close();
    int  affect_rows();
    const char*  error_msg();

    int  exe_sql(const string& sql_, db_each_row_callback_i* cb_ = NULL);
    int  exe_sql(const string& sql_, vector<vector<string> >& ret_data_);
    int  exe_sql(const string& sql_, vector<vector<string> >& ret_data_, vector<string>& col_names_);
private:
    db_ops_i*           m_db_ops;
};

}
#endif

