//! 消息定义
#ifndef _COMMON_MSG_DEF_H_
#define _COMMON_MSG_DEF_H_

#include "net/codec.h"

using namespace ff;

struct login_t
{
    struct in_t: public msg_i
    {
        in_t():
            msg_i("login_t::in_t")
        {}
        virtual string encode()
        {
            return (init_encoder() << uid << passwd).get_buff();
        }
        virtual void decode(const string& src_buff_)
        {
            init_decoder(src_buff_) >> uid >> passwd;
        }
        
        long    uid;
        string  passwd;
    };
    struct out_t: public msg_i
    {
        out_t():
            msg_i("login_t::out_t")
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
};

struct logout_t
{
    struct in_t: public msg_i
    {
        in_t():
            msg_i("logout_t::in_t")
        {}
        virtual string encode()
        {
            return (init_encoder() << uid << passwd).get_buff();
        }
        virtual void decode(const string& src_buff_)
        {
            init_decoder(src_buff_) >> uid >> passwd;
        }
        
        long    uid;
        string  passwd;
    };
    struct out_t: public msg_i
    {
        out_t():
            msg_i("logout_t::out_t")
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

struct unicast_t
{
    struct in_t: public msg_i
    {
        in_t():
            msg_i("unicast_t::in_t")
        {}
        virtual string encode()
        {
            return (init_encoder() << uid << content).get_buff();
        }
        virtual void decode(const string& src_buff_)
        {
            init_decoder(src_buff_) >> uid >> content;
        }
        
        long    uid;
        string  content;
    };
    struct out_t: public msg_i
    {
        out_t():
            msg_i("unicast_t::out_t"),
            ret(true)
        {}
        virtual string encode()
        {
            return (init_encoder() << ret).get_buff();
        }
        virtual void decode(const string& src_buff_)
        {
            init_decoder(src_buff_) >> ret;
        }
        
        bool ret;
    };
};


struct broadcast_t
{
    struct in_t: public msg_i
    {
        in_t():
            msg_i("broadcast_t::in_t")
        {}
        virtual string encode()
        {
            return (init_encoder() << content).get_buff();
        }
        virtual void decode(const string& src_buff_)
        {
            init_decoder(src_buff_) >> content;
        }
        
        string  content;
    };
    struct out_t: public msg_i
    {
        out_t():
            msg_i("broadcast_t::out_t"),
            ret(true)
        {}
        virtual string encode()
        {
            return (init_encoder() << ret).get_buff();
        }
        virtual void decode(const string& src_buff_)
        {
            init_decoder(src_buff_) >> ret;
        }
        
        bool ret;
    };
};

struct common_msg_t
{
    struct in_t: public msg_i
    {
        in_t():
        msg_i("common_msg_t::in_t")
        {}
        virtual string encode()
        {
            return (init_encoder() << uid << content).get_buff();
        }
        virtual void decode(const string& src_buff_)
        {
            init_decoder(src_buff_) >> uid >> content;
        }
        long    uid;
        string  content;
    };
    struct out_t: public msg_i
    {
        out_t():
        msg_i("common_msg_t::out_t"),
        ret(true)
        {}
        virtual string encode()
        {
            return (init_encoder() << ret).get_buff();
        }
        virtual void decode(const string& src_buff_)
        {
            init_decoder(src_buff_) >> ret;
        }
        bool ret;
    };
};
#endif
