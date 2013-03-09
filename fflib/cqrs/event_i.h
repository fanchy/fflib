
#ifndef _FF_EVENT_I_
#define _FF_EVENT_I_

#include "base/fftype.h"

namespace ff
{

class event_i: public type_i
{
public:
    virtual ~event_i(){}
};

template<typename T>
class event_t: public auto_type_t<event_i, T> 
{
public:
    virtual ~event_t(){}
};

}
#endif
