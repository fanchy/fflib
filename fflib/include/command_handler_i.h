
#ifndef _FF_COMMAD_HANDLER_I_
#define _FF_COMMAD_HANDLER_I_

namespace ff
{

class command_handler_i: public type_i
{
public:
    virtual ~command_handler_i(){}
};

template<typename T>
class command_handler_t: public auto_type_t<command_handler_i, T> 
{
public:
    virtual ~command_handler_t(){}
};

}
#endif
