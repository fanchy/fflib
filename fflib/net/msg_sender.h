//! 消息发送器

#ifndef _MSG_SENDER_H_
#define _MSG_SENDER_H_

namespace ff {

#include "net/socket_i.h"
#include "net/message.h"
#include "net/codec.h"
//! #include "zlib_util.h"

class msg_sender_t
{
public:
    static void send(socket_ptr_t socket_ptr_, uint16_t cmd_, const string& str_)
    {
        if (socket_ptr_)
        {
            message_head_t h(cmd_);
            h.size = str_.size();
            string dest((const char*)&h, sizeof(h));
            dest += str_;
            socket_ptr_->async_send(dest);
        }
    }
    static void send(socket_ptr_t socket_ptr_, uint16_t cmd_, codec_i& msg_)
    {
        if (socket_ptr_)
        {
            string body = msg_.encode();
            message_head_t h(cmd_);
            h.size = body.size();
            string dest((const char*)&h, sizeof(h));
            dest += body;

            socket_ptr_->async_send(dest);
        }
    }
    static void send(socket_ptr_t socket_ptr_, const string& str_)
    {
        if (socket_ptr_)
        {
            socket_ptr_->async_send(str_);
        }
    }
    static void send_to_client(socket_ptr_t socket_ptr_, codec_i& msg_)
    {
        if (socket_ptr_)
        {
            string body = msg_.encode();
            message_head_t h(0);
            h.size = body.size();
            string dest((const char*)&h, sizeof(h));
            dest += body;
            socket_ptr_->async_send(body);
        }
    }
};

}
#endif
