
#include "ai/ffai.h"
#include "net/codec.h"
using namespace ff;

#include <stdio.h>

struct monster_t: public fftype_t<monster_t>
{
    monster_t():
        ffstate_machine(this)
    {
    }
    ffstate_machine_t<monster_t> ffstate_machine;
};


class event_C_t: public fftype_t<event_C_t>
{
public:
    void dumy()
    {
        printf("event_C_t::dumy........\n");
    }
};

class state_B_t;
class state_A_t: public ff_state_t<monster_t>
{
public:
    ~state_A_t()
    {
        printf("state_A_t::~state_A_t....\n");
    }
    virtual void enter(fsm_t*)
    {
        printf("state_A_t::enter....[%p]\n", this);
    }
    virtual void update(fsm_t* p);
    virtual void exit(fsm_t*)
    {
        printf("state_A_t::exit....[%p]\n", this);
    }
    virtual void handle(fsm_t*, type_i& event_){}
};
class state_B_t: public ff_state_t<monster_t>
{
public:
    ~state_B_t()
    {
        printf("state_B_t::~state_B_t....\n");
    }
    virtual void enter(fsm_t*)
    {
        printf("state_B_t::enter....[%p]\n", this);
    }
    virtual void update(fsm_t* p)
    {
        printf("state_B_t::update....[%p]\n", this);
        p->change(shared_ptr_t<state_A_t>(new state_A_t()));
    }
    virtual void exit(fsm_t*)
    {
        printf("state_B_t::exit....[%p]\n", this);
    }
    virtual void handle(fsm_t*, type_i& event_)
    {
        printf("state_B_t::handle....[%p, event=%s]\n", this, event_.get_type_name().c_str());
        if (event_.cast<event_C_t>())
        {
            event_.cast<event_C_t>()->dumy();
        }
    }
};
void state_A_t::update(fsm_t* p)
{
    printf("state_A_t::update....[%p]\n", this);
    p->change(shared_ptr_t<state_B_t>(new state_B_t()));
}

int main(int argc, char* argv[])
{
    option_t<int> p;
    if (!p)
    {
        p.init();
        *p = 100;
    }
    if (p)
        printf("p=%d\n", *p);
    return 0;
    monster_t monster;
    monster.ffstate_machine.change(shared_ptr_t<state_A_t>(new state_A_t()));
    for (int i = 0; i < 100; ++i)
    {
        sleep(3);
        printf("\n----------------------------\n");
        event_C_t event;
        monster.ffstate_machine.handle(event);
        monster.ffstate_machine.update();
        singleton_t<obj_summary_t>::instance().dump("./obj.txt");
    }
    return 0;
}
/* 输出
 *  ./app_ai 
state_A_t::enter....[0x9274008]

----------------------------
state_A_t::update....[0x9274008]
state_A_t::exit....[0x9274008]
state_B_t::enter....[0x9274058]

----------------------------
state_B_t::handle....[0x9274058, event=event_C_t]
event_C_t::dumy........
state_B_t::update....[0x9274058]
state_A_t::~state_A_t....
state_B_t::exit....[0x9274058]
state_A_t::enter....[0x9274088]
 */