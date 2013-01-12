//! 消息定义
#ifndef _MSG_DEF_H_
#define _MSG_DEF_H_

#include "codec.h"

using namespace ff;

struct test_msg_t: public msg_i
{
    test_msg_t():
        msg_i("test")
    {}
    virtual string encode()
    {
        return (init_encoder() << val).get_buff();
    }
    virtual void decode(const string& src_buff_)
    {
        init_decoder(src_buff_) >> val;
    }
    
    int val;
};
#endif
