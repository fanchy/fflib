#include <iostream>
using namespace std;

#include "msg_def.h"
#include "log/log.h"
#include "rpc/msg_bus.h"

using namespace ff;

class test_service_t: public msg_handler_i
{
public:
    int m_a;
    test_service_t(int a):m_a(a){}
    ~test_service_t(){}
    int handle_broken(socket_ptr_t sock_)
    {
        cout <<m_a <<" handle broken\n";
        sock_->safe_delete();
        return 0;
    }
    int handle_msg(const message_t& msg_, socket_ptr_t sock_)
    {
        test_msg_t test_msg;
        test_msg.decode(msg_.get_body());
        cout <<m_a << " test_service_t::handle msg:" << test_msg.val <<"\n";
        /*
        test_msg.val = m_a;
        if (m_a == 1) {
            msg_sender_t::send(sock_, 100, test_msg);
        }
        else
            msg_sender_t::send(sock_, 100, test_msg);
         */
        return 0;
    }
};
test_service_t test_service(1), test_service2(2);

void foo(test_msg_t& in_msg_, rpc_callcack_t<test_msg_t>& cb_) {}
int foo1(const int& a){return 0;}
int foo2(const int& a, const char* p_){return 0;}
int foo3(const int& a, const char* p_, string b){return 0;}
int foo4(const int& a, const char* p_, string b, long c){return 0;}
int foo5(const int& a, const char* p_, string b, long c, const short*){return 0;}
int foo6(const int& a, const char* p_, string b, long c, const test_service_t*, char aa){return 0;}

int foo7(const int& a, const char* p_, string b, long c, const test_service_t*, char aa, float cc){return 0;}
int foo8(const int& a, const char* p_, string b, long c, const test_service_t*, char aa, float cc, double dd){return 0;}
int foo9(const int& a, const char* p_, string b, long c, const test_service_t*, char aa, float cc, double, long long){return 0;}

struct foo_t
{
public:
    void dumy(){}
    void foo(test_msg_t& in_msg_, rpc_callcack_t<test_msg_t>& cb_)
    {
        cout << "foo_t::foo done:" << in_msg_.val ++ << "\n";
        usleep(1000* (in_msg_.val % 100));
        cb_(in_msg_);
    }
    int foo1(const int& a){return 0;}
    int foo2(const int& a, const char* p_){return 0;}
    int foo3(const int& a, const char* p_, string b){return 0;}
    int foo4(const int& a, const char* p_, string b, long c){return 0;}
    int foo5(const int& a, const char* p_, string b, long c, const short*){return 0;}
    int foo6(const int& a, const char* p_, string b, long c, const test_service_t*, char aa){return 0;}
    
    int foo7(const int& a, const char* p_, string b, long c, const test_service_t*, char aa, float cc){return 0;}
    int foo8(const int& a, const char* p_, string b, long c, const test_service_t*, char aa, float cc, double dd){return 0;}
    int foo9(const int& a, const char* p_, string b, long c, const test_service_t*, char aa, float cc, double, long long){return 0;}
};

rpc_service_t* g_rpc_service = NULL;
int main(int argc, char* argv[])
{
    
    char buff[128];
    snprintf(buff, sizeof(buff), "tcp://%s:%s", "127.0.0.1", "10241");

    //acceptor_i* acceptor = net_factory_t::listen(buff, &test_service);
    //cout <<"acceptor:" << acceptor <<"\n";
    //test_service2.handle_broken(NULL);

    //task_binder_t::gen(&foo);
    //task_binder_t::gen(&foo1);
    
    
    task_binder_t::gen(&foo1, 1);
    task_binder_t::gen(&foo2, 1, "");
    task_binder_t::gen(&foo3, 1, "", "aa");
    task_binder_t::gen(&foo4, 1, "", "aa", 100);
    task_binder_t::gen(&foo5, 1, "", "aa", 100, (const short*)NULL);
    task_binder_t::gen(&foo6, 1, "", "aa", 100, (const test_service_t*)NULL, 'a');
    task_binder_t::gen(&foo7, 1, "", "aa", 100, (const test_service_t*)NULL, 'a', 0.1);
    task_binder_t::gen(&foo8, 1, "", "aa", 100, (const test_service_t*)NULL, 'a', 0.1, 0.2);
    task_binder_t::gen(&foo9, 1, "", string("aa"), (long)100, (const test_service_t*)NULL, 'a', (float)0.1, 0.3, (long long)11111);
    
    foo_t f;
    task_binder_t::gen(&foo_t::dumy, &f);
    task_binder_t::gen(&foo_t::foo1, (foo_t*)&f, 1);
    task_binder_t::gen(&foo_t::foo2, &f, 1, "");
    task_binder_t::gen(&foo_t::foo3, &f, 1, "", "aa");
    task_binder_t::gen(&foo_t::foo4, &f, 1, "", "aa", 100);
    task_binder_t::gen(&foo_t::foo5, &f, 1, "", "aa", 100, (const short*)NULL);
    task_binder_t::gen(&foo_t::foo6, &f, 1, "", "aa", 100, (const test_service_t*)NULL, 'a');
    task_binder_t::gen(&foo_t::foo7, &f, 1, "", "aa", 100, (const test_service_t*)NULL, 'a', 0.1);
    task_binder_t::gen(&foo8, 1, "", "aa", 100, (const test_service_t*)NULL, 'a', 0.1, 0.2);
    task_binder_t::gen(&foo9, 1, "", string("aa"), (long)100, (const test_service_t*)NULL, 'a', (float)0.1, 0.3, (long long)11111);
    
//    rpc_service_t rpc_service(1, 1);
//    rpc_service.reg(&foo);
//    rpc_service.reg(&foo_t::foo, &f);
    
    ffrpc_t msg_bus;
    msg_bus.open("tcp://127.0.0.1:10241");
    /*
    msg_bus.create_service_group("teeeest");
    msg_bus.create_service("teeeest", 1)
           .bind_service(&f)
           .reg(&foo_t::foo);
    */
    msg_bus.create_service_group("fooooo");
    msg_bus.create_service("fooooo", 1)
    .bind_service(&f)
    .reg(&foo_t::foo);
    
    struct lambda_t
    {
        static void callback(test_msg_t& in_msg_)
        {
            cout <<"oh nice\n";
            if (in_msg_.val < 1000) {
                g_rpc_service->async_call(in_msg_, &lambda_t::callback);
            }
            else
            {
                g_rpc_service = NULL;
            }
        }

    };
    test_msg_t test_msg;
    test_msg.val = 1;
    cout <<"\n\n";
    sleep(1);
    g_rpc_service = msg_bus.get_service_group("fooooo")->get_service(1);
    g_rpc_service->async_call(test_msg, &lambda_t::callback);
    while(g_rpc_service)
        sleep(1);
    /*
    socket_ptr_t skt = net_factory_t::connect("tcp://127.0.0.1:10241", (msg_handler_i*)&msg_bus);
    cout << "skt:" << skt <<"\n";
    g_rpc_service = &msg_bus.create_service("test", 1);//(1, 1, skt);
    //g_rpc_service->set_socket(skt);
    
    test_msg_t test_msg;
    test_msg.val = 1;
    struct lambda_t
    {
        static void callback(test_msg_t& in_msg_)
        {
            cout <<"oh nice\n";
            call(in_msg_);
        }
        static void call(test_msg_t& test_msg)
        {
            if (test_msg.val < 10)
            {
            test_msg.val ++;
            g_rpc_service->async_call(test_msg, &lambda_t::callback);
            }
            else
            {
                g_rpc_service = NULL;
            }
        }
    };
    g_rpc_service->async_call(test_msg, &lambda_t::callback);

    test_msg_t test_msg;
    test_msg.val  = 10241;
    msg_sender_t::send(skt, 100, test_msg);

    sleep(1);
    //skt->close();
    while (g_rpc_service) {
        usleep(1000);
    }
         */ 
    cout <<"oh end\n";
    
    return 0;
}
