#include <iostream>
using namespace std;

#include "logic_msg_def.h"
#include "rpc/msg_bus.h"
#include "log/log.h"

using namespace ff;

struct login_t
{
    struct in_t: public client_msg_i
    {
        in_t():
            client_msg_i("login_t::in_t")
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
    struct out_t: public client_msg_i
    {
        out_t():
        client_msg_i("login_t::out_t")
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

class client_service_t: public msg_handler_i
{
    int handle_broken(socket_ptr_t sock_)
    {
        cout <<"client_service_t::handle_broken\n";
        return 0;
    }
    int handle_msg(const message_t& msg_, socket_ptr_t sock_)
    {
        cout <<"client_service_t::handle_msg\n";
        return 0;
    }

};

int main(int argc, char* argv[])
{
    char buff[128];
    snprintf(buff, sizeof(buff), "tcp://%s:%s", "127.0.0.1", "20241");


    client_service_t client_service;
    socket_ptr_t socket_ptr = net_factory_t::connect(buff, &client_service);

    assert(socket_ptr);
    login_t::in_t in;
    in.uid = 123;
    msg_sender_t::send(socket_ptr, 0, in);

    signal_helper_t::wait();
    singleton_t<msg_bus_t>::instance().close();
    cout <<"\noh end\n";
    
    return 0;
}
