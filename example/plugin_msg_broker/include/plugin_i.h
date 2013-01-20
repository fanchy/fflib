#ifndef _PLUGIN_H_
#define _PLUGIN_H_

#include "channel.h"
#include "net/message.h"

using namespace ff;

class plugin_i
{
public:
    virtual ~plugin_i(){}
    virtual int start() = 0;
    virtual int stop() = 0;

    virtual int handle_broken(channel_ptr_t channel_) = 0;
    virtual int handle_msg(const message_t& msg_, channel_ptr_t channel_) = 0;
};

typedef plugin_i* plugin_ptr_t;
typedef int (*handle_channel_msg_func_t)(const message_t& msg_, channel_ptr_t);
typedef int (*handle_channel_broken_func_t)(channel_ptr_t);

#define HANDLE_CHANNEL_MSG       "handle_channel_msg"
#define HANDLE_CHANNEL_BROKEN "handle_channel_broken"
#endif
