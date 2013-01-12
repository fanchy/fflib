//! 消息定义
#ifndef _MSG_DEF_H_
#define _MSG_DEF_H_

#include "codec.h"

using namespace ff;

struct echo_t
{
    struct in_t: public msg_i
    {
        in_t():
            msg_i("echo_t::in_t")
        {}
        virtual string encode()
        {
            return (init_encoder() << value).get_buff();
        }
        virtual void decode(const string& src_buff_)
        {
            init_decoder(src_buff_) >> value;
        }
        
        string value;
    };
    struct out_t: public msg_i
    {
        out_t():
            msg_i("echo_t::out_t")
        {}
        virtual string encode()
        {
            return (init_encoder() << value).get_buff();
        }
        virtual void decode(const string& src_buff_)
        {
            init_decoder(src_buff_) >> value;
        }
        
        string value;
    };
};

#endif
