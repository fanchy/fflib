#include <iostream>
using namespace std;

#include "impl/entity_user.h"
#include "impl/event_store_mem.h"

int main(int argc, char* argv[])
{
    event_store_mem_t event_store;
    {
        entity_user_t     entity_user(1122334, &event_store);
        entity_user.inc_gold(100);
        entity_user.inc_level(100);
        cout <<"user 1:" << entity_user.encode() <<"\n";
    }
    {
        entity_user_t     entity_user(1122334, &event_store);
        cout <<"user 2:" << entity_user.encode() <<"\n";
        entity_user.inc_gold(100);
        entity_user.inc_level(100);
        cout <<"user 3:" << entity_user.encode() <<"\n";
    }
    entity_user_t     entity_user(1122334, &event_store);
    cout <<"user 4:" << entity_user.encode() <<"\n";
    return 0;
}

