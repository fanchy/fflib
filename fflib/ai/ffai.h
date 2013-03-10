#ifndef _FF_AI_H_
#define _FF_AI_H_

#include "base/fftype.h"
#include "base/singleton.h"
#include "base/smart_ptr.h"

#include <string>
using namespace std;

namespace ff
{

template <typename T>
class ffstate_machine_t;

template<typename T>
class ff_state_t
{
public:
    typedef ffstate_machine_t<T> fsm_t;
public:
    virtual ~ff_state_t(){}
    virtual void enter(fsm_t*)  {}
    virtual void update(fsm_t*) {}
    virtual void exit(fsm_t*)   {}
    virtual void handle(fsm_t*, type_i& event_){}
};

template <typename T>
class ffstate_machine_t
{
public:
    typedef shared_ptr_t<ff_state_t<T> >  ff_state_ptr_t;
public:
    ffstate_machine_t(T* obj_):
        m_dest_obj(obj_)
    {}

    virtual ~ffstate_machine_t(){}
    T*   get_owener() { return m_dest_obj; }
    void update()
    {
        if(m_current_state)   m_current_state->update(this);
    }

    void handle(type_i& event_)
    {
        if (m_current_state) m_current_state->handle(this, event_);
    }

    void  change(ff_state_ptr_t new_state_)
    {
        m_previous_state = m_current_state;
        if (m_current_state)
        {
            m_current_state->exit(this);
        }

        m_current_state = new_state_;
        if (m_current_state) m_current_state->enter(this);
    }

    ff_state_ptr_t&  current_state()  {     return m_current_state;    }
    ff_state_ptr_t&  previous_state() {     return m_previous_state;   }
    
private:
    T*                  m_dest_obj;
    ff_state_ptr_t      m_previous_state;
    ff_state_ptr_t      m_current_state;
};

}
#endif

