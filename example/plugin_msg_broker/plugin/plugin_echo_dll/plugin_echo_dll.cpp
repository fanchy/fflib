#include "msg_broker_iterface.h"
#include <iostream>
using namespace std;

extern "C"{

int handle_channel_msg(const message_t& msg_, channel_ptr_t channel_)
{
    cout <<"handle_channel_msg:" << msg_.get_body() << "\n";
    channel_->async_send("oh nice");
    return 0;
}

int handle_channel_broken(channel_ptr_t channel_)
{
    cout <<"handle_channel_broken:\n";
    channel_->close();
    delete channel_;
    return 0;
}

}
