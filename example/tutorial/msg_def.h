
//! 消息定义
#ifndef _MSG_DEF_H_
#define _MSG_DEF_H_

#include "net/codec.h"

using namespace ff;

struct user_login_t
{
    struct in_t: public msg_i
    {
        in_t():
        msg_i("user_login_t::in_t")
        {}
        string encode()
        {
            return (init_encoder() << uid << value).get_buff();
        }
        void decode(const string& src_buff_)
        {
            init_decoder(src_buff_) >> uid >> value;
        }
        long   uid;
        string value;
    };
    struct out_t: public msg_i
    {
        out_t():
        msg_i("user_login_t::out_t")
        {}
        string encode()
        {
            return (init_encoder() << value).get_buff();
        }
        void decode(const string& src_buff_)
        {
            init_decoder(src_buff_) >> value;
        }
        bool value;
    };
};

struct config_t
{
    struct in_t: public msg_i
    {
        in_t():
        msg_i("config_t::in_t")
        {}
        string encode()
        {
            return (init_encoder() << server_type << server_id).get_buff();
        }
        void decode(const string& src_buff_)
        {
            init_decoder(src_buff_) >> server_type >> server_id;
        }
        int server_type;
        int server_id;
    };
    struct out_t: public msg_i
    {
        out_t():
        msg_i("config_t::out_t")
        {}
        string encode()
        {
            return (init_encoder() << value).get_buff();
        }
        void decode(const string& src_buff_)
        {
            init_decoder(src_buff_) >> value;
        }
        map<string, string> value;
    };
};


struct user_online_t
{
    struct in_t: public msg_i
    {
        in_t():
        msg_i("user_online_t::in_t")
        {}
        string encode()
        {
            return (init_encoder() << uid).get_buff();
        }
        void decode(const string& src_buff_)
        {
            init_decoder(src_buff_) >> uid;
        }
        long uid;
    };
    struct out_t: public msg_i
    {
        out_t():
        msg_i("user_online_t::out_t")
        {}
        string encode()
        {
            return (init_encoder() << value).get_buff();
        }
        void decode(const string& src_buff_)
        {
            init_decoder(src_buff_) >> value;
        }
        bool value;
    };
};


struct force_user_offline_t
{
    struct in_t: public msg_i
    {
        in_t():
        msg_i("force_user_offline_t::in_t")
        {}
        string encode()
        {
            return (init_encoder() << uid).get_buff();
        }
        void decode(const string& src_buff_)
        {
            init_decoder(src_buff_) >> uid;
        }
        long uid;
    };
    struct out_t: public msg_i
    {
        out_t():
        msg_i("force_user_offline_t::out_t")
        {}
        string encode()
        {
            return (init_encoder() << value).get_buff();
        }
        void decode(const string& src_buff_)
        {
            init_decoder(src_buff_) >> value;
        }
        bool value;
    };
};


struct reload_config_t
{
    struct in_t: public msg_i
    {
        in_t():
        msg_i("reload_config_t::in_t")
        {}
        string encode()
        {
            return (init_encoder()).get_buff();
        }
        void decode(const string& src_buff_)
        {
            init_decoder(src_buff_);
        }
    };
    struct out_t: public msg_i
    {
        out_t():
        msg_i("reload_config_t::out_t")
        {}
        string encode()
        {
            return (init_encoder() << value).get_buff();
        }
        void decode(const string& src_buff_)
        {
            init_decoder(src_buff_) >> value;
        }
        bool value;
    };
};


struct word_count_t
{
    struct in_t: public msg_i
    {
        in_t():
        msg_i("word_count_t::in_t")
        {}
        string encode()
        {
            return (init_encoder() << str).get_buff();
        }
        void decode(const string& src_buff_)
        {
            init_decoder(src_buff_) >> str;
        }
        string str;
    };
    struct out_t: public msg_i
    {
        out_t():
        msg_i("word_count_t::out_t")
        {}
        string encode()
        {
            return (init_encoder() << value).get_buff();
        }
        void decode(const string& src_buff_)
        {
            init_decoder(src_buff_) >> value;
        }
        map<int, int> value;
    };
};

#endif
