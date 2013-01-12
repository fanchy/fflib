
#include <iostream>
using namespace std;

#include "rank_obj.h"
#include "rank_obj_mgr.h"
#include "rank_system.h"

int main(int argc, char* argv[])
{
    rank_obj_mgr_t rank_obj_mgr;
    rank_system_t  rank_system(&rank_obj_mgr);
    
    enum
    {
        LEVEL_RANK = 1
    };
    //!   等级排行榜， 排名前一百个
    rank_system.create_ranklist(LEVEL_RANK, 100);
    ::system("pause");
    return 0;
}
