#include <iostream>
using namespace std;

#include "msg_def.h"
#include "rpc/ffrpc.h"
#include "log/log.h"

using namespace ff;

int g_count = 0;
int g_index = 1;

int main(int argc, char* argv[])
{/*
    map<int, string> src;src[13] = "qqq";src[14] = "eee";
    map<int, string> dest;
    bin_encoder_t enc;
    bin_decoder_t dec;
    
    string str = (enc.init(100) << src).get_buff();
    dec.init(str.substr(8)) >>dest;
    cout << dest.size() <<" " << dest[13] << " " << dest[14] << "\n";
    return 0;
   */ 
    if (argc > 1)
    {
        g_index = atoi(argv[1]);
    }
    char buff[128];
    snprintf(buff, sizeof(buff), "tcp://%s:%s", "127.0.0.1", "10241");

    assert(0 == singleton_t<ffrpc_t>::instance().open(buff));

    assert(singleton_t<ffrpc_t>::instance().get_service_group("echo") && "echo service group not exist");

    assert(singleton_t<ffrpc_t>::instance().get_service_group("echo")->get_service(g_index) && "echo servie 1 not exist");

    sleep(1);

    struct lambda_t
    {
        static void callback(echo_t::out_t& msg_)
        {
            if (g_count % 5000 == 0)
            {
                logwarn((FF, "%d, index[%d] echo callback msg value<%s>", g_index, g_count, msg_.value.c_str()));
            }

            if (++g_count > 500000) {
                //return;
            }
            
            echo_t::in_t in;
            in.value = "XXX_echo_test_XXX";
            
            singleton_t<ffrpc_t>::instance().get_service_group("echo")->get_service(g_index)->async_call(in, &lambda_t::callback);
        }
        
    };

    for (int i = 0; i < 1; ++i)
    {
    	echo_t::in_t in;
        in.value = "XXX_echo_test_XXX";
    	singleton_t<ffrpc_t>::instance().get_service_group("echo")->get_service(g_index)->async_call(in, &lambda_t::callback);
    }

    signal_helper_t::wait();
    singleton_t<ffrpc_t>::instance().close();
    cout <<"\noh end\n";
    
    return 0;
}
