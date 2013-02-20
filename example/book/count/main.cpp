
#include "count/ffcount.h"

using namespace ff;
#include <stdio.h>

int main(int argc, char* argv[])
{
    event_log_t el("dumy", "A,B");el.def(100, "pp");
    
    ffcount_t ffcount;
    
    int ret = ffcount.add_event(el);
    printf("add event ret=%d\n", ret);
    
    ffcount.query("select * from dumy");
    
    return 0;
}
