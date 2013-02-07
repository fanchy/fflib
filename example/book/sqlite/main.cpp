
#include "db/ffdb.h"
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
}
int main(int argc, char* argv[])
{
    ffdb_t ffdb;
    if (ffdb.connect("/test.db"))
    {
        printf("connect error:%s, %d\n", ffdb.error_msg(), ffdb.is_connected());
        return 1;
    }
    if (ffdb.exe_sql("CREATE TABLE  foodb (id int, name varchar(20), age int)"))
    {
        printf("exe error:%s\n", ffdb.error_msg());
    }
    if (ffdb.exe_sql("insert into foodb values(200, 'nihao', 25)"))
    {
        printf("exe error:%s\n", ffdb.error_msg());
    }
    vector<vector<string> > ret_data;
    if (ffdb.exe_sql("select  * from foodb", ret_data))
    {
        printf("exe error:%s\n", ffdb.error_msg());
    }
    dump(ret_data);
    
    return 0;
}
