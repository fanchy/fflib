
#include "db/ffdb.h"
#include "db/ffcrud.h"
using namespace ff;
#include <stdio.h>

void dump(vector<vector<string> >& ret_data)
{
    for (size_t i = 0; i < ret_data.size(); ++i)
    {
        printf("row[%u] begin======= ", i);
        for (size_t j = 0; j < ret_data[i].size(); ++j)
        {
            printf(" %s", ret_data[i][j].c_str());
        }
        printf(" =======row[%u] end\n", i);
    }
    ret_data.clear();
}

struct foo_t: public ffcrud_t<foo_t>
{
    foo_t():
        a(167),
        b("ddd"),
        m_c(11.22){}
    int a;
    string b;
    double& c()  { return m_c; }
    double m_c;
};
int main(int argc, char* argv[])
{
    ffdb_t ffdb;
    foo_t foo;
    vector<vector<string> > ret_data;
    if (ffdb.connect("/test.db"))
    {
        printf("connect error:%s, %d\n", ffdb.error_msg(), ffdb.is_connected());
        return 1;
    }
    if (ffdb.exe_sql("CREATE TABLE  dumy (A int, c float, b varchar(200), primary key (A))"))
    {
        printf("exe error:%s\n", ffdb.error_msg());
    }
    if (ffdb.exe_sql("select * from dumy", ret_data))
    {
        printf("exe error:%s\n", ffdb.error_msg());
    }
    dump(ret_data);
    
    
    ffcrud_register_t<foo_t>::bind_table("dumy", "A")
                                .def(&foo_t::a, "A")
                                .def(&foo_t::c, "C")
                                .def(&foo_t::b, "B");
    
    
    printf("foo insert:<%s>\n", foo.insert_sql().c_str());
    printf("foo select:<%s>\n", foo.select_sql().c_str());
    printf("foo update:<%s>\n", foo.update_sql().c_str());
    printf("foo delete:<%s>\n", foo.del_sql().c_str());
    //foo.insert(ffdb);
    foo.select(ffdb);
    if (ffdb.exe_sql("select * from dumy", ret_data))
    {
        printf("exe error:%s\n", ffdb.error_msg());
    }
    dump(ret_data);
    
    foo.m_c = 23.99;
    
    if (ffdb.exe_sql(foo.update_sql(), ret_data))
    {
        printf("exe error:%s\n", ffdb.error_msg());
    }
    if (ffdb.exe_sql("select * from dumy", ret_data))
    {
        printf("exe error:%s\n", ffdb.error_msg());
    }
    dump(ret_data);
    /*
    if (ffdb.exe_sql(foo.del_sql(), ret_data))
    {
        printf("exe error:%s\n", ffdb.error_msg());
    }
    if (ffdb.exe_sql("select * from dumy", ret_data))
    {
        printf("exe error:%s\n", ffdb.error_msg());
    }
    dump(ret_data);
    */
    //if (ffdb.exe_sql("SELECT C FROM foo WHERE A = 167 limit 1;", ret_data))
    if (ffdb.exe_sql(foo.select_sql(), ret_data))
    {
        printf("exe error:%s\n", ffdb.error_msg());
    }
    dump(ret_data);
    foo.b += "a";
    foo.update(ffdb, &foo_t::b);
    if (ffdb.exe_sql(foo.select_sql(), ret_data))
    {
        printf("exe error:%s\n", ffdb.error_msg());
    }
    dump(ret_data);
    return 0;
}
