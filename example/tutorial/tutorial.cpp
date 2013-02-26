
#include <iostream>
#include <map>
using namespace std;

#include "msg_def.h"
#include "rpc/ffrpc.h"
#include "log/log.h"

using namespace ff;
class login_server_t
{
public:
    void verify(user_login_t::in_t& in_msg_, rpc_callcack_t<user_login_t::out_t>& cb_)
    {
        user_login_t::out_t out;
        out.value = true;
        cb_(out);
    }
};

void test_async_request_reply()
{
    login_server_t login_server;
    singleton_t<ffrpc_t>::instance().create_service("login_server", 1)
        .bind_service(&login_server)
        .reg(&login_server_t::verify);
    
    struct lambda_t
    {
        static void callback(user_login_t::out_t& msg_, socket_ptr_t socket_)
        {
            if (true == msg_.value)
            {
                //! socket_->send_msg("login ok");
            }
            else
            {
                //! socket_->send_msg("login failed");
            }
        }
    };
    
    user_login_t::in_t in;
    in.uid  = 520;
    in.value = "ILoveYou";
    socket_ptr_t flash_socket = NULL;//! TODO

    singleton_t<ffrpc_t>::instance()
    .get_service_group("login_server_t")
    ->get_service(1)
    ->async_call(in, binder_t::callback(&lambda_t::callback, flash_socket));
}

class super_server_t
{
public:
    void get_config(config_t::in_t& in_msg_, rpc_callcack_t<config_t::out_t>& cb_)
    {
        config_t::out_t out;
        out.value["oh"] = "nice";
        cb_(out);
    }
};

void test_sync_request_reply()
{
    super_server_t super_server;
    singleton_t<ffrpc_t>::instance().create_service("super_server", 1)
        .bind_service(&super_server)
        .reg(&super_server_t::get_config);
    
    rpc_future_t<config_t::out_t> rpc_future;
    
    config_t::in_t in;
    in.server_type = 1;
    in.server_id   = 1;
    const config_t::out_t& out = rpc_future.call(
        singleton_t<ffrpc_t>::instance().get_service_group("super_server")
        ->get_service(1), in);
    cout << out.value.size() <<"\n";
    //std::foreach(out.value.begin(), out.value.end(), fuctor_xx);
}

class gateway_server_t
{
public:
    void force_user_offline(force_user_offline_t::in_t& in_msg_, rpc_callcack_t<force_user_offline_t::out_t>& cb_)
    {
        //! close user socket
        force_user_offline_t::out_t out;
        out.value = true;
        cb_(out);
    }
};
class session_server_t
{
public:
    void user_login(user_online_t::in_t& in_msg_, rpc_callcack_t<user_online_t::out_t>& cb_)
    {
        //! close user socket
        user_online_t::out_t out;
        out.value = true;
        cb_(out);
    }
};

void test_p2p()
{
    gateway_server_t gateway_server;
    singleton_t<ffrpc_t>::instance().create_service("gateway_server", 1)
        .bind_service(&gateway_server)
        .reg(&gateway_server_t::force_user_offline);
    
    user_online_t::in_t in;
    in.uid = 520;
    
    struct lambda_t
    {
        static void callback_TODO(user_login_t::out_t& out_)
        {}
        static void callback_TODO2(user_login_t::out_t& out_)
        {}
    };
    singleton_t<ffrpc_t>::instance()
        .get_service_group("session_server")
        ->get_service(1)
        ->async_call(in, &lambda_t::callback_TODO);

    session_server_t session_server;
    singleton_t<ffrpc_t>::instance().create_service("session_server", 1)
        .bind_service(&session_server)
        .reg(&session_server_t::user_login);

    force_user_offline_t::in_t in2;
    in2.uid = 520;
    
    singleton_t<ffrpc_t>::instance()
    .get_service_group("gateway_server")
        ->get_service(1)
    ->async_call(in2, &lambda_t::callback_TODO2);
}

class scene_server_t
{
public:
    void reload_config(reload_config_t::in_t& in_msg_, rpc_callcack_t<reload_config_t::out_t>& cb_)
    {
        //! close user socket
        reload_config_t::out_t out;
        out.value = true;
        cb_(out);
    }
};
void test_multi_broadcast()
{
    scene_server_t scene_server;
    singleton_t<ffrpc_t>::instance().create_service("scene_server", 1)
        .bind_service(&scene_server)
        .reg(&scene_server_t::reload_config);
    
    struct lambda_t
    {
        static void callback_TODO(reload_config_t::out_t& out_)
        {}
        static void reload_config(rpc_service_t* rs_)
        {
            reload_config_t::in_t in;
            rs_->async_call(in, callback_TODO);
        }
    };
    
    singleton_t<ffrpc_t>::instance()
        .get_service_group("scene_server")
        ->foreach(&lambda_t::reload_config);
}

class worker_t
{
public:
    void word_count(word_count_t::in_t& in_msg_, rpc_callcack_t<word_count_t::out_t>& cb_)
    {
        //! close user socket
        word_count_t::out_t out;
        for (size_t i = 0; i < in_msg_.str.size(); ++i)
        {
            map<int, int>::iterator it = out.value.find(in_msg_.str[i]);
            if (it != out.value.end())
            {
                it->second += 1;
            }
            else
            {
                out.value[in_msg_.str[i]] = 1;
            }
        }
        cb_(out);
    }
};

void test_map_reduce()
{
    worker_t worker;
    
    for (int i = 0; i < 5; ++i)
    {
        singleton_t<ffrpc_t>::instance().create_service("worker", 1)
            .bind_service(&worker)
            .reg(&worker_t::word_count);
    }
    
    
    struct lambda_t
    {
        static void reduce(word_count_t::out_t& msg_, map<int, int>* result_, size_t* size_)
        {
            for (map<int, int>::iterator it = msg_.value.begin(); it != msg_.value.end(); ++it)
            {
                map<int, int>::iterator it2 = result_->find(it->first);
                if (it2 != result_->end())
                {
                    it2->second += it->second;
                }
                else
                {
                    (*result_)[it->first] = it->second;
                }
            }
            if (-- size_ == 0)
            {
                //reduce end!!!!!!!!!!!!!!!!
                delete result_;
                delete size_;
            }
        }
        
        static void do_map(const char** p, size_t size_)
        {
            map<int, int>* result  = new map<int, int>();
            size_t*    dest_size   = new size_t();
            *dest_size = size_;
            
            for (size_t i = 0; i < size_; ++i)
            {
                word_count_t::in_t in;
                in.str = p[i];
                
                singleton_t<ffrpc_t>::instance()
                    .get_service_group("worker")
                    ->get_service(1 + i % singleton_t<ffrpc_t>::instance().get_service_group("worker")->size())
                    ->async_call(in, binder_t::callback(&lambda_t::reduce, result, dest_size));
            }
        }
    };
    const char* str_vec[] = {"oh nice", "oh fuck", "oh no", "oh dear", "oh wonderful", "oh bingo"};
    lambda_t::do_map(str_vec, 6);
}

int main(int argc, char* argv[])
{
    return 0;
}
