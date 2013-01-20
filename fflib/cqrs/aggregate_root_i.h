
#ifndef _FF_AGGREGATE_ROOT_I_
#define _FF_AGGREGATE_ROOT_I_

#include <list>
using namespace std;

#include "cqrs/event_i.h"

namespace ff
{

typedef long guid_t;

class aggregate_root_i: public type_i
{
    typedef list<event_i*> event_list_t;
public:
    aggregate_root_i():m_id(0){}
    virtual ~aggregate_root_i(){}

    virtual guid_t get_id() const     { return m_id; }
    virtual void   set_id(guid_t id_) { m_id = id_;  }

    void  add_event(event_i* e_) {m_events.push_back(e_);}
    const event_list_t& get_events() const { return m_events; }
    void  clear_event()
    {
        for (event_list_t::iterator it = m_events.begin(); it != m_events.end(); ++it)
        {
            delete *it;
        }
        m_events.clear();
    }

    template<typename T>
    void add_event(const T& event_)
    {
        this->add_event(new T(event_));
    }
protected:
    guid_t          m_id;
    event_list_t  m_events;
};

template <typename T>
class aggregate_root_t: public auto_type_t<aggregate_root_i, T>
{
public:
    virtual ~ aggregate_root_t(){}
};

#define AR aggregate_root_t
}
#endif
