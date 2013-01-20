//! 消息定义
#ifndef _LOGIC_MSG_DEF_H_
#define _LOGIC_MSG_DEF_H_

#include "net/codec.h"

using namespace ff;

struct user_login_t: public client_msg_i
{
    user_login_t():
        client_msg_i("user_login_t")
    {}
    virtual string encode()
    {
        return (init_encoder() << uid).get_buff();
    }
    virtual void decode(const string& src_buff_)
    {
        init_decoder(src_buff_) >> uid;
    }
    
    long uid;
};

struct user_logout_t: public client_msg_i
{
    user_logout_t():
        client_msg_i("user_login_t")
    {}
    virtual string encode()
    {
        return (init_encoder() << uid).get_buff();
    }
    virtual void decode(const string& src_buff_)
    {
        init_decoder(src_buff_) >> uid;
    }
    
    long uid;
};
#endif
