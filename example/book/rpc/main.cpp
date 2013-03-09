#include <stdio.h>

#include "count/ffcount.h"
#include "rpc/broker_application.h"
#include "base/daemon_tool.h"
#include "base/arg_helper.h"
#include "base/strtool.h"
#include "base/smart_ptr.h"

using namespace ff;
bool g_run = false;

struct test_msg_t
{
    struct in_t: public ffmsg_t<test_msg_t::in_t>
    {
        virtual string encode()
        {
            return (init_encoder() << uid).get_buff() ;
        }
        virtual void decode(const string& src_buff_)
        {
            init_decoder(src_buff_) >> uid;
        }
        long    uid;
    };
    typedef ffmsg_bool_t out_t;
};
struct update_msg_t
{
    struct in_t: public ffmsg_t<update_msg_t::in_t>
    {
        virtual string encode()
        {
            return (init_encoder() << uid).get_buff() ;
        }
        virtual void decode(const string& src_buff_)
        {
            init_decoder(src_buff_) >> uid;
        }
        long    uid;
    };
    typedef ffmsg_bool_t out_t;
};

class logic_service_t
{
public:
    logic_service_t(ffrpc_t* p, int i):ffrpc(p), m_index(i){}
    int test(test_msg_t::in_t& in_msg_, rpc_callcack_t<test_msg_t::out_t>& cb_)
    {
        sleep(2);
        printf("in logic_service_t::test[index=%d], 被client调用 uid[%ld]\n", m_index, in_msg_.uid);

        test_msg_t::out_t out;
        out.value = true;
        cb_(out);
        
        struct lambda_t
        {
            static void async_callback(update_msg_t::out_t& msg_)
            {
                sleep(2);
                printf("logic_service_t 接收db_service的返回值 ret_bool=[%d]\n", msg_.value);
            }
        };
        update_msg_t::in_t in;
        in.uid = in_msg_.uid;
        ffrpc->async_call("db_service", 1 + in_msg_.uid % ffrpc->service_num("db_service"), in, &lambda_t::async_callback);
        return 0;
    }
    
    ffrpc_t* ffrpc;
    int      m_index;
};

class db_service_t
{
public:
    db_service_t(ffrpc_t* p, int i):ffrpc(p), m_index(i){}
    int update(update_msg_t::in_t& in_msg_, rpc_callcack_t<update_msg_t::out_t>& cb_)
    {
        sleep(2);
        printf("in db_service_t::update[index=%d], 被logic_service调用uid[%ld]\n", m_index, in_msg_.uid);
        update_msg_t::out_t out;
        out.value = true;
        cb_(out);
        return 0;
    }
    
    ffrpc_t* ffrpc;
    int      m_index;
};

int start_logic_service(ffrpc_t& ffrpc, logic_service_t& service, arg_helper_t* arg_helper_, int index_)
{
    //printf("start_logic_service index[%d] begin\n", index_);
    assert(0 == ffrpc.open(arg_helper_->get_option_value("-l")) && "can't connnect to broker");

    ffrpc.create_service("logic_service", index_)
            .bind_service(&service)
            .reg(&logic_service_t::test);
    //printf("start_logic_service index[%d] end\n", index_);
    return 0;
}
int start_db_service(ffrpc_t& ffrpc, db_service_t& service, arg_helper_t* arg_helper_, int index_)
{
    //printf("start_db_service index[%d] begin\n", index_);
    assert(0 == ffrpc.open(arg_helper_->get_option_value("-l")) && "can't connnect to broker");

    ffrpc.create_service("db_service", index_)
            .bind_service(&service)
            .reg(&db_service_t::update);
    //printf("start_db_service index[%d] end\n", index_);
    return 0;
}

struct foo_t
{
    foo_t()
    {
        printf("XXX ... foo...\n");
    }
    ~foo_t()
    {
        printf("XXX ... ~foo...\n");
    }
};

void thread1(shared_ptr_t<foo_t> p)
{
    for (int i = 0 ; i < 1000000;)
    {
        shared_ptr_t<foo_t> v= p;
        weak_ptr_t<foo_t> wp1 = v;
        assert(v);
        i += 1;
    }
}
int test_ptr()
{
    shared_ptr_t<foo_t> sp1(new foo_t);
    weak_ptr_t<foo_t> wp1;
    wp1 = sp1;
    printf("wp1[%p], weak_ref[%ld], ref[%ld]\n", wp1.lock().get(), wp1.ger_ref_count()->m_weak_ref_count.value(),
                wp1.ger_ref_count()->m_ref_count.value());
    thread_t thread;
    thread.create_thread(task_binder_t::gen(&thread1, sp1), 100);
    
    do
    {
        sleep(1);
        printf("wp1[%p], weak_ref[%ld], ref[%ld]\n", wp1.lock().get(), wp1.ger_ref_count()->m_weak_ref_count.value(),
                wp1.ger_ref_count()->m_ref_count.value());
    }while(wp1.ger_ref_count()->m_weak_ref_count.value() > 2);
    thread.join();
    sp1.reset();
    printf("wp1[%p], weak_ref[%ld], ref[%ld]\n", wp1.lock().get(), wp1.ger_ref_count()->m_weak_ref_count.value(),
                wp1.ger_ref_count()->m_ref_count.value());
    return 0;
}
int main(int argc, char* argv[])
{
    
    if (argc == 1)
    {
        printf("usage: app -broker -client -l tcp://127.0.0.1:10241 -service db_service@1-4,logic_service@1-4\n");
        return 1;
    }
    arg_helper_t arg_helper(argc, argv);
    if (arg_helper.is_enable_option("-broker"))
    {
        broker_application_t::run(argc, argv);
    }

    if (arg_helper.is_enable_option("-d"))
    {
        daemon_tool_t::daemon();
    }
    
    vector<string> all_service_name;
    strtool_t::split(arg_helper.get_option_value("-service"), all_service_name, ",");
    
    vector<ffrpc_t*>            vt_rpc;
    vector<db_service_t*>       vt_db_service;
    vector<logic_service_t*>    vt_logic_service;
    for (size_t i = 0; i < all_service_name.size(); ++i)
    {
        vector<string> opts;
        strtool_t::split(all_service_name[i], opts, "@");
        int index_begin = 0;
        int index_end   = 0;
        if (opts.size() > 1)
        {
            vector<string> vt_index;
            strtool_t::split(opts[1], vt_index, "-");
            if (vt_index.empty() == false)
            {
                index_begin = ::atoi(vt_index[0].c_str());
                if (vt_index.size() > 1)
                {
                    index_end = ::atoi(vt_index[1].c_str());
                }
            }
        }
        if (index_end < index_begin) index_end = index_begin;
        printf("service includes<%s:%d-%d>\n", opts[0].c_str(), index_begin, index_end);
        
        for (int i = index_begin; i <= index_end; ++i)
        {
            ffrpc_t* ffrpc = new ffrpc_t();
            vt_rpc.push_back(ffrpc);
            if (opts[0] == "db_service")
            {
                db_service_t* service = new db_service_t(ffrpc, i);
                start_db_service(*ffrpc, *service, &arg_helper, i);
                vt_db_service.push_back(service);
            }
            else if (opts[0] == "logic_service")
            {
                logic_service_t* service = new logic_service_t(ffrpc, i);
                start_logic_service(*ffrpc, *service, &arg_helper, i);
                vt_logic_service.push_back(service);
            }
        }
    }
    
    if (arg_helper.is_enable_option("-client"))
    {
        ffrpc_t ffrpc;
        for (int i = 1; i < 100000; ++i)
        {
            sleep(1);
            printf("client 准备调用logic_service[index=%d]\n", i);

            assert(0 == ffrpc.open(arg_helper.get_option_value("-l")) && "can't connnect to broker");

            test_msg_t::in_t in;
            in.uid = i;

            test_msg_t::out_t out;
            ffrpc.call("logic_service", 1 + in.uid % ffrpc.service_num("logic_service"), in, out);
            sleep(8);
            printf("logic_service[index=%d] 调用返回=%d\n", i, out.value);
        }
        ffrpc.close();
    }
    signal_helper_t::wait();
    for (size_t i = 0; i < vt_rpc.size(); ++i)
    {
        vt_rpc[i]->close();
        delete vt_rpc[i];
    }
    for (size_t i = 0; i < vt_db_service.size(); ++i)
    {
        delete vt_db_service[i];
    }
    for (size_t i = 0; i < vt_logic_service.size(); ++i)
    {
        delete vt_logic_service[i];
    }
    return 0;
}
