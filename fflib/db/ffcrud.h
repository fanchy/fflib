#ifndef _FFCRUD_H_
#define _FFCRUD_H_

#include <string>
#include <sstream>
#include <vector>
#include <map>
using namespace std;
#include <string.h>
#include <stdio.h>
#include "base/singleton.h"
#include "base/strtool.h"
#include "db/ffdb.h"

namespace ff
{

class db_field_info_i
{
public:
    virtual ~db_field_info_i(){}
    virtual string  get() = 0;
    virtual int     set(const string& val_) = 0;

    db_field_info_i&  set_data(void* p_) { m_obj_ptr = p_; return *this;}
    template<typename T>
    T* get_data() { return (T*)m_obj_ptr; }
    virtual long key() = 0;
protected:
    void* m_obj_ptr;;
};

template<typename CLASS_TYPE, typename RET>
class db_field_property_impl_t: public db_field_info_i
{
public:
    db_field_property_impl_t(RET CLASS_TYPE::* p_):
        m_field_ptr(p_)
    {
    }
    virtual string  get()
    {
        stringstream ss;
        string ret;
        ss << (get_data<CLASS_TYPE>()->*m_field_ptr);
        ss >> ret;
        return ret;
    }
    virtual int     set(const string& val_)
    {
        stringstream ss;
        ss << val_;
        ss >> (get_data<CLASS_TYPE>()->*m_field_ptr);
        return 0;
    }
    virtual long key()
    { 
        long ret = 0;
        ::memcpy(&ret, &m_field_ptr, sizeof(ret));
        return ret;
    }
private:
    RET CLASS_TYPE::*  m_field_ptr;
};

template<typename CLASS_TYPE, typename RET>
class db_field_function_impl_t: public db_field_info_i
{
    typedef RET (CLASS_TYPE::*dest_func_t)();
public:
    db_field_function_impl_t(dest_func_t p_):
        m_field_ptr(p_)
    {
    }
    virtual string  get()
    {
        stringstream ss;
        string ret;
        ss << (get_data<CLASS_TYPE>()->*m_field_ptr)();
        ss >> ret;
        return ret;
    }
    virtual int     set(const string& val_)
    {
        stringstream ss;
        ss << val_;
        ss >> (get_data<CLASS_TYPE>()->*m_field_ptr)();
        return 0;
    }
    virtual long key()
    { 
        long ret = 0;
        ::memcpy(&ret, &m_field_ptr, sizeof(ret));
        return ret;
    }
private:
    dest_func_t m_field_ptr;
};

class db_field_impl_factory_t
{
public:
    template<typename CLASS_TYPE, typename RET>
    static db_field_info_i* alloc(RET CLASS_TYPE::* p_)
    {
        return new db_field_property_impl_t<CLASS_TYPE, RET>(p_);
    }
    template<typename CLASS_TYPE, typename RET>
    static db_field_info_i* alloc(RET (CLASS_TYPE::*dest_func)())
    {
        return new db_field_function_impl_t<CLASS_TYPE, RET>(dest_func);
    }
};

//!  注册接口
template <typename T>
class ffcrud_register_t
{
public:
    struct field_reg_info_t
    {
        field_reg_info_t(string name_ = "", db_field_info_i* p = NULL):
            field_name(name_),
            field_data(p)
        {}
        ~field_reg_info_t()
        {
            if (field_data)
            {
                delete field_data;
                field_data = NULL;
            }
        }
        string field_name;
        db_field_info_i* field_data;
    };
    typedef vector<field_reg_info_t*> field_reg_info_vt_t;
    typedef typename field_reg_info_vt_t::iterator field_reg_info_it_t;

public:
    virtual ~ffcrud_register_t()
    {
        for (typename map<long, field_reg_info_t*>::iterator it = m_index_for_field.begin(); it != m_index_for_field.end(); ++it)
        {
            delete it->second;
        }
        m_primarykey_fields.clear();
        m_normal_fields.clear();
        m_index_for_field.clear();
    }
    static ffcrud_register_t& bind_table(const string& table_name_, const string& primarykeys_)
    {
        return singleton_t<ffcrud_register_t<T> >::instance().bind_table_and_primarykey(table_name_, primarykeys_);
    }
    ffcrud_register_t& bind_table_and_primarykey(const string& table_name_, const string& primarykeys_)
    {
        m_table_name = table_name_;
        vector<string> str_vt;
        strtool_t::split(primarykeys_, str_vt, ",");
        for (size_t i = 0; i < str_vt.size(); ++i)
        {
            field_reg_info_t* info = new field_reg_info_t(str_vt[i]);
            m_primarykey_fields.push_back(info);
        }
        return *this;
    }
    
    template<typename RET>
    ffcrud_register_t& def(RET T::* p_, const string& field_name_)
    {
        field_reg_info_it_t it = this->find(m_primarykey_fields, field_name_);
        if (it != m_primarykey_fields.end())
        {
            (*it)->field_data = db_field_impl_factory_t::alloc(p_);
            m_index_for_field[(*it)->field_data->key()] = (*it);
            return *this;
        }
        
        m_normal_fields.push_back(new field_reg_info_t(field_name_, db_field_impl_factory_t::alloc(p_)));
        m_index_for_field[m_normal_fields.back()->field_data->key()] = (m_normal_fields[m_normal_fields.size() - 1]);
        return *this;
    }
    template<typename RET>
    ffcrud_register_t& def(RET (T::*p_)(), const string& field_name_)
    {
        field_reg_info_it_t it = this->find(m_primarykey_fields, field_name_);
        if (it != m_primarykey_fields.end())
        {
            (*it)->field_data = db_field_impl_factory_t::alloc(p_);
            m_index_for_field[(*it)->field_data->key()] = (*it);
            return *this;
        }
        
        m_normal_fields.push_back(new field_reg_info_t(field_name_, db_field_impl_factory_t::alloc(p_)));
        m_index_for_field[m_normal_fields.back()->field_data->key()] = (m_normal_fields[m_normal_fields.size() - 1]);
        
        return *this;
    }
    field_reg_info_it_t find(field_reg_info_vt_t& vt_, const string& name_)
    {
        field_reg_info_it_t it = vt_.begin();
        for (; it != vt_.end(); ++it)
        {
            if ((*it)->field_name == name_)
            {
                return it;
            }
        }
        return it;
    }
    
    const string& get_table_name()                     { return m_table_name; }
    const field_reg_info_vt_t& get_primarykey_fields() { return m_primarykey_fields; }
    const field_reg_info_vt_t& get_normal_fields()     { return m_normal_fields; }
    
    field_reg_info_t* get_field_by_key(long key_)
    {
        typename map<long, field_reg_info_t*>::iterator it = m_index_for_field.find(key_);
        if (it != m_index_for_field.end())
        {
            return it->second;
        }
        return NULL;
    }
    
private:
    string                              m_table_name;
    field_reg_info_vt_t                 m_primarykey_fields;
    field_reg_info_vt_t                 m_normal_fields;
    map<long, field_reg_info_t*>        m_index_for_field;
};

#define CRUD_INFO(X) singleton_t<ffcrud_register_t<X> >::instance() 

template<typename T>
class ffcrud_t
{
public:
    virtual ~ffcrud_t(){}
    string insert_sql()
    {
        string sql = "INSERT INTO ";
        sql += CRUD_INFO(T).get_table_name() + " (";
        string val = " (";

        for (size_t i = 0; i < CRUD_INFO(T).get_primarykey_fields().size(); ++i)
        {
            sql += CRUD_INFO(T).get_primarykey_fields()[i]->field_name + ", ";
            val += "'" + CRUD_INFO(T).get_primarykey_fields()[i]->field_data->set_data(this).get() + "', ";
        }
        
        for (size_t i = 0; i < CRUD_INFO(T).get_normal_fields().size(); ++i)
        {
            sql += CRUD_INFO(T).get_normal_fields()[i]->field_name + ", ";
            val += "'" + CRUD_INFO(T).get_normal_fields()[i]->field_data->set_data(this).get() + "', ";
        }
        sql[sql.length() - 2] = ')';
        val[val.length() - 2] = ')';
        val[val.length() - 1] = ';';
        sql += "VALUES" + val;
        return sql;
    }
    string select_sql()
    {
        string sql = "SELECT ";
        
        for (size_t i = 0; i < CRUD_INFO(T).get_normal_fields().size(); ++i)
        {
            if (i != 0) sql += ", ";
            sql += CRUD_INFO(T).get_normal_fields()[i]->field_name + " ";
        }
        sql += "FROM " + CRUD_INFO(T).get_table_name() + " WHERE ";
        
        for (size_t i = 0; i < CRUD_INFO(T).get_primarykey_fields().size(); ++i)
        {
            if (i != 0) sql += ", ";
            sql += CRUD_INFO(T).get_primarykey_fields()[i]->field_name + " = '" + 
                   CRUD_INFO(T).get_primarykey_fields()[i]->field_data->set_data(this).get() + "'";
        }
        sql += " limit 1;";
        return sql;
    }
    string update_sql()
    {
        string sql = "UPDATE ";
        sql += CRUD_INFO(T).get_table_name() + " SET ";
        
        for (size_t i = 0; i < CRUD_INFO(T).get_normal_fields().size(); ++i)
        {
            if (i != 0) sql += ", ";
            sql += CRUD_INFO(T).get_normal_fields()[i]->field_name + " = '" +
                            CRUD_INFO(T).get_normal_fields()[i]->field_data->set_data(this).get() + "'";
        }
        sql += " WHERE ";
        for (size_t i = 0; i < CRUD_INFO(T).get_primarykey_fields().size(); ++i)
        {
            if (i != 0) sql += ", ";
            sql += CRUD_INFO(T).get_primarykey_fields()[i]->field_name + " = '" + 
                   CRUD_INFO(T).get_primarykey_fields()[i]->field_data->set_data(this).get() + "'";
        }
        return sql;
    }
    string del_sql()
    {
        string sql = "DELETE FROM ";
        sql += CRUD_INFO(T).get_table_name();
        
        sql += " WHERE ";
        for (size_t i = 0; i < CRUD_INFO(T).get_primarykey_fields().size(); ++i)
        {
            if (i != 0) sql += ", ";
            sql += CRUD_INFO(T).get_primarykey_fields()[i]->field_name + " = '" + 
                   CRUD_INFO(T).get_primarykey_fields()[i]->field_data->set_data(this).get() + "'";
        }
        return sql;
    }
    
    
    int insert(ffdb_t& ffdb)
    {
        return ffdb.exe_sql(this->insert_sql());
    }
    int select(ffdb_t& ffdb)
    {
        vector<vector<string> > ret_data;
        if (ffdb.exe_sql(this->select_sql(), ret_data) || ret_data.size() != 1)
        {
            return -1;
        }
        for (size_t i = 0; i < CRUD_INFO(T).get_normal_fields().size(); ++i)
        {
            CRUD_INFO(T).get_normal_fields()[i]->field_data->set_data(this).set(ret_data[0][i]);
        }
        return 0;
    }
	int select_sql(ffdb_t& ffdb, const string& sql_)
    {
        vector<vector<string> > ret_data;
        if (ffdb.exe_sql(sql_, ret_data) || ret_data.size() != 1)
        {
            return -1;
        }
        for (size_t i = 0; i < CRUD_INFO(T).get_normal_fields().size(); ++i)
        {
            CRUD_INFO(T).get_normal_fields()[i]->field_data->set_data(this).set(ret_data[0][i]);
        }
        return 0;
    }
    int update(ffdb_t& ffdb)
    {
        return ffdb.exe_sql(this->update_sql());
    }
    int del(ffdb_t& ffdb)
    {
        return ffdb.exe_sql(this->del_sql());
    }
    
    /************** 高级接口  *************************/
    string update_sql(vector<db_field_info_i*> vt_)
    {
        string sql = "UPDATE ";
        sql += CRUD_INFO(T).get_table_name() + " SET ";
        
        for (size_t i = 0; i < vt_.size(); ++i)
        {
            typename ffcrud_register_t<T>::field_reg_info_t* info = CRUD_INFO(T).get_field_by_key(vt_[i]->key());
            
            if (NULL == info || NULL == info->field_data)
            {
                return "";
            }
            if (i != 0) sql += ", ";
            
            sql += info->field_name + " = '" + info->field_data->set_data(this).get() + "'";
        }
        sql += " WHERE ";
        for (size_t i = 0; i < CRUD_INFO(T).get_primarykey_fields().size(); ++i)
        {
            if (i != 0) sql += ", ";
            sql += CRUD_INFO(T).get_primarykey_fields()[i]->field_name + " = '" + 
                   CRUD_INFO(T).get_primarykey_fields()[i]->field_data->set_data(this).get() + "'";
        }
        return sql;
    }
    string select_sql(vector<db_field_info_i*> vt_)
    {
        string sql = "SELECT ";
        size_t n = 0;
        for (size_t i = 0; i < CRUD_INFO(T).get_primarykey_fields().size(); ++i)
        {
            if (n++ != 0) sql += ", ";
            sql += CRUD_INFO(T).get_primarykey_fields()[i]->field_name + " ";
        }
        for (size_t i = 0; i < CRUD_INFO(T).get_normal_fields().size(); ++i)
        {
            if (n++ != 0) sql += ", ";
            sql += CRUD_INFO(T).get_normal_fields()[i]->field_name + " ";
        }
        sql += "FROM " + CRUD_INFO(T).get_table_name() + " WHERE ";
        
        for (size_t i = 0; i < vt_.size(); ++i)
        {
            typename ffcrud_register_t<T>::field_reg_info_t* info = CRUD_INFO(T).get_field_by_key(vt_[i]->key());
            
            if (NULL == info || NULL == info->field_data)
            {
                return "";
            }
            if (i != 0) sql += " AND ";
            sql += info->field_name + " = '" + info->field_data->set_data(this).get() + "'";
        }
        return sql;
    }
    void destory(vector<db_field_info_i*>& args_)
    {
        for (size_t i =0 ;i < args_.size(); ++i)
        {
            delete args_[i];
        }
        args_.clear();
    }
    template <typename ARG1>
    int update(ffdb_t& ffdb, ARG1 arg1)
    {
        vector<db_field_info_i*> args;
        args.push_back(db_field_impl_factory_t::alloc(arg1));
        string sql = update_sql(args);
        destory(args);
        if (false == sql.empty())
        {
            return ffdb.exe_sql(sql);
        }
        return -1;
    }
    template <typename ARG1, typename ARG2>
    int update(ffdb_t& ffdb, ARG1 arg1, ARG2 arg2)
    {
        vector<db_field_info_i*> args;
        args.push_back(db_field_impl_factory_t::alloc(arg1));
        args.push_back(db_field_impl_factory_t::alloc(arg2));
        string sql = update_sql(args);
        destory(args);
        if (false == sql.empty())
        {
            return ffdb.exe_sql(sql);
        }
        return -1;
    }
    template <typename ARG1, typename ARG2, typename ARG3>
    int update(ffdb_t& ffdb, ARG1 arg1, ARG2 arg2, ARG3 arg3)
    {
        vector<db_field_info_i*> args;
        args.push_back(db_field_impl_factory_t::alloc(arg1));
        args.push_back(db_field_impl_factory_t::alloc(arg2));
        args.push_back(db_field_impl_factory_t::alloc(arg3));
        string sql = update_sql(args);
        destory(args);
        if (false == sql.empty())
        {
            return ffdb.exe_sql(sql);
        }
        return -1;
    }
    template <typename ARG1, typename ARG2, typename ARG3, typename ARG4>
    int update(ffdb_t& ffdb, ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4)
    {
        vector<db_field_info_i*> args;
        args.push_back(db_field_impl_factory_t::alloc(arg1));
        args.push_back(db_field_impl_factory_t::alloc(arg2));
        args.push_back(db_field_impl_factory_t::alloc(arg3));
        args.push_back(db_field_impl_factory_t::alloc(arg4));
        string sql = update_sql(args);
        destory(args);
        if (false == sql.empty())
        {
            return ffdb.exe_sql(sql);
        }
        return -1;
    }
    template <typename ARG1, typename ARG2, typename ARG3, typename ARG4, typename ARG5>
    int update(ffdb_t& ffdb, ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4, ARG5 arg5)
    {
        vector<db_field_info_i*> args;
        args.push_back(db_field_impl_factory_t::alloc(arg1));
        args.push_back(db_field_impl_factory_t::alloc(arg2));
        args.push_back(db_field_impl_factory_t::alloc(arg3));
        args.push_back(db_field_impl_factory_t::alloc(arg4));
        args.push_back(db_field_impl_factory_t::alloc(arg5));
        string sql = update_sql(args);
        destory(args);
        if (false == sql.empty())
        {
            return ffdb.exe_sql(sql);
        }
        return -1;
    }
    template <typename ARG1, typename ARG2, typename ARG3, typename ARG4, typename ARG5, typename ARG6>
    int update(ffdb_t& ffdb, ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4, ARG5 arg5, ARG6 arg6)
    {
        vector<db_field_info_i*> args;
        args.push_back(db_field_impl_factory_t::alloc(arg1));
        args.push_back(db_field_impl_factory_t::alloc(arg2));
        args.push_back(db_field_impl_factory_t::alloc(arg3));
        args.push_back(db_field_impl_factory_t::alloc(arg4));
        args.push_back(db_field_impl_factory_t::alloc(arg5));
        args.push_back(db_field_impl_factory_t::alloc(arg6));
        string sql = update_sql(args);
        destory(args);
        if (false == sql.empty())
        {
            return ffdb.exe_sql(sql);
        }
        return -1;
    }
    template <typename ARG1, typename ARG2, typename ARG3, typename ARG4, typename ARG5, typename ARG6, typename ARG7>
    int update(ffdb_t& ffdb, ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4, ARG5 arg5, ARG6 arg6, ARG7 arg7)
    {
        vector<db_field_info_i*> args;
        args.push_back(db_field_impl_factory_t::alloc(arg1));
        args.push_back(db_field_impl_factory_t::alloc(arg2));
        args.push_back(db_field_impl_factory_t::alloc(arg3));
        args.push_back(db_field_impl_factory_t::alloc(arg4));
        args.push_back(db_field_impl_factory_t::alloc(arg5));
        args.push_back(db_field_impl_factory_t::alloc(arg6));
        args.push_back(db_field_impl_factory_t::alloc(arg7));
        string sql = update_sql(args);
        destory(args);
        if (false == sql.empty())
        {
            return ffdb.exe_sql(sql);
        }
        return -1;
    }
    template <typename ARG1, typename ARG2, typename ARG3, typename ARG4, typename ARG5, typename ARG6, typename ARG7, typename ARG8>
    int update(ffdb_t& ffdb, ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4, ARG5 arg5, ARG6 arg6, ARG7 arg7, ARG8 arg8)
    {
        vector<db_field_info_i*> args;
        args.push_back(db_field_impl_factory_t::alloc(arg1));
        args.push_back(db_field_impl_factory_t::alloc(arg2));
        args.push_back(db_field_impl_factory_t::alloc(arg3));
        args.push_back(db_field_impl_factory_t::alloc(arg4));
        args.push_back(db_field_impl_factory_t::alloc(arg5));
        args.push_back(db_field_impl_factory_t::alloc(arg6));
        args.push_back(db_field_impl_factory_t::alloc(arg7));
        args.push_back(db_field_impl_factory_t::alloc(arg8));
        string sql = update_sql(args);
        destory(args);
        if (false == sql.empty())
        {
            return ffdb.exe_sql(sql);
        }
        return -1;
    }
    template <typename ARG1, typename ARG2, typename ARG3, typename ARG4, typename ARG5, typename ARG6, typename ARG7, typename ARG8, typename ARG9>
    int update(ffdb_t& ffdb, ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4, ARG5 arg5, ARG6 arg6, ARG7 arg7, ARG8 arg8, ARG9 arg9)
    {
        vector<db_field_info_i*> args;
        args.push_back(db_field_impl_factory_t::alloc(arg1));
        args.push_back(db_field_impl_factory_t::alloc(arg2));
        args.push_back(db_field_impl_factory_t::alloc(arg3));
        args.push_back(db_field_impl_factory_t::alloc(arg4));
        args.push_back(db_field_impl_factory_t::alloc(arg5));
        args.push_back(db_field_impl_factory_t::alloc(arg6));
        args.push_back(db_field_impl_factory_t::alloc(arg7));
        args.push_back(db_field_impl_factory_t::alloc(arg8));
        args.push_back(db_field_impl_factory_t::alloc(arg9));
        string sql = update_sql(args);
        destory(args);
        if (false == sql.empty())
        {
            return ffdb.exe_sql(sql);
        }
        return -1;
    }
    
    template <typename ARG1>
    int select_all(ffdb_t& ffdb, vector<T>& ret_, ARG1 arg1)
    {
        vector<vector<string> > ret_data;
        vector<db_field_info_i*> args;
        args.push_back(db_field_impl_factory_t::alloc(arg1));

        if (ffdb.exe_sql(this->select_sql(args), ret_data))
        {
            return -1;
        }
        for (size_t n = 0; n < ret_data.size(); ++n)
        {
            ret_.push_back(T());
            for (size_t i = 0; i < CRUD_INFO(T).get_primarykey_fields().size(); ++i)
            {
                CRUD_INFO(T).get_primarykey_fields()[i]->field_data->set_data(&(ret_[ret_.size()-1])).set(ret_data[n][i]);
            }
            for (size_t i = 0; i < CRUD_INFO(T).get_normal_fields().size(); ++i)
            {
                CRUD_INFO(T).get_normal_fields()[i]->field_data->set_data(&(ret_[ret_.size()-1])).set(ret_data[n][i + CRUD_INFO(T).get_primarykey_fields().size()]);
            }
        }
        return 0;
    }
    template <typename ARG1, typename ARG2>
    int select_all(ffdb_t& ffdb, vector<T>& ret_, ARG1 arg1, ARG2 arg2)
    {
        vector<vector<string> > ret_data;
        vector<db_field_info_i*> args;
        args.push_back(db_field_impl_factory_t::alloc(arg1));
        args.push_back(db_field_impl_factory_t::alloc(arg2));

        if (ffdb.exe_sql(this->select_sql(args), ret_data))
        {
            printf("Sql:%s\n", ffdb.error_msg());
            return -1;
        }
        
        for (size_t n = 0; n < ret_data.size(); ++n)
        {
            ret_.push_back(T());
            for (size_t i = 0; i < CRUD_INFO(T).get_primarykey_fields().size(); ++i)
            {
                CRUD_INFO(T).get_primarykey_fields()[i]->field_data->set_data(&(ret_[ret_.size()-1])).set(ret_data[n][i]);
            }
            for (size_t i = 0; i < CRUD_INFO(T).get_normal_fields().size(); ++i)
            {
                CRUD_INFO(T).get_normal_fields()[i]->field_data->set_data(&(ret_[ret_.size()-1])).set(ret_data[n][i + CRUD_INFO(T).get_primarykey_fields().size()]);
            }
        }
        return 0;
    }
    template <typename ARG1, typename ARG2, typename ARG3>
    int select_all(ffdb_t& ffdb, vector<T>& ret_, ARG1 arg1, ARG2 arg2, ARG3 arg3)
    {
        vector<vector<string> > ret_data;
        vector<db_field_info_i*> args;
        args.push_back(db_field_impl_factory_t::alloc(arg1));
        args.push_back(db_field_impl_factory_t::alloc(arg2));
        args.push_back(db_field_impl_factory_t::alloc(arg3));

        if (ffdb.exe_sql(this->select_sql(args), ret_data))
        {
            return -1;
        }
        for (size_t n = 0; n < ret_data.size(); ++n)
        {
            ret_.push_back(T());
            for (size_t i = 0; i < CRUD_INFO(T).get_primarykey_fields().size(); ++i)
            {
                CRUD_INFO(T).get_primarykey_fields()[i]->field_data->set_data(&(ret_[ret_.size()-1])).set(ret_data[n][i]);
            }
            for (size_t i = 0; i < CRUD_INFO(T).get_normal_fields().size(); ++i)
            {
                CRUD_INFO(T).get_normal_fields()[i]->field_data->set_data(&(ret_[ret_.size()-1])).set(ret_data[n][i + CRUD_INFO(T).get_primarykey_fields().size()]);
            }
        }
        return 0;
    }
    template <typename ARG1, typename ARG2, typename ARG3, typename ARG4>
    int select_all(ffdb_t& ffdb, vector<T>& ret_, ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4)
    {
        vector<vector<string> > ret_data;
        vector<db_field_info_i*> args;
        args.push_back(db_field_impl_factory_t::alloc(arg1));
        args.push_back(db_field_impl_factory_t::alloc(arg2));
        args.push_back(db_field_impl_factory_t::alloc(arg3));
        args.push_back(db_field_impl_factory_t::alloc(arg4));

        if (ffdb.exe_sql(this->select_sql(args), ret_data))
        {
            return -1;
        }
        for (size_t n = 0; n < ret_data.size(); ++n)
        {
            ret_.push_back(T());
            for (size_t i = 0; i < CRUD_INFO(T).get_primarykey_fields().size(); ++i)
            {
                CRUD_INFO(T).get_primarykey_fields()[i]->field_data->set_data(&(ret_[ret_.size()-1])).set(ret_data[n][i]);
            }
            for (size_t i = 0; i < CRUD_INFO(T).get_normal_fields().size(); ++i)
            {
                CRUD_INFO(T).get_normal_fields()[i]->field_data->set_data(&(ret_[ret_.size()-1])).set(ret_data[n][i + CRUD_INFO(T).get_primarykey_fields().size()]);
            }
        }
        return 0;
    }
};
}

#endif

