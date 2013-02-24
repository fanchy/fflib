#ifndef _NET_FACTORY_H_
#define _NET_FACTORY_H_

#include "net/acceptor_impl.h"
#include "net/gateway_acceptor.h"
#include "net/http_acceptor.h"
#include "net/epoll_impl.h"
#include "connector.h"
#include "base/singleton.h"
#include "base/performance_daemon.h"
#include "base/thread.h"
#include "net/codec.h"
#include "net/msg_sender.h"

namespace ff {

class net_factory_t
{
public:
    struct global_data_t
    {
        volatile bool      started_flag;
        task_queue_pool_t* tg;
        thread_t           thread;
        epoll_impl_t       epoll;
        vector<acceptor_i*>all_acceptor;
        global_data_t():
            started_flag(false),
            tg(NULL),
            epoll()
        {
        }
        ~ global_data_t()
        {
            stop();
        }
        static void run_epoll(void* e_)
        {
            global_data_t* p = (global_data_t*)e_;
            p->epoll.event_loop();
        }
        void start(int thread_num_ = 2)
        {
            if (false == started_flag)
            {
                assert(thread_num_ > 0);
                started_flag = true;
                tg = new task_queue_pool_t(thread_num_);
                thread.create_thread(task_t(&run_epoll, this), 1);
                thread.create_thread(task_queue_pool_t::gen_task(tg), thread_num_);
            }
        }
        void stop()
        {
            if (true == started_flag)
            {
                for (size_t i = 0; i < all_acceptor.size(); ++i)
                {
                    all_acceptor[i]->close();
                }
                for (size_t i = 0; i < all_acceptor.size(); ++i)
                {
                    delete all_acceptor[i];
                }
                all_acceptor.clear();
                tg->close();
                epoll.close();
                thread.join();
                delete tg;
                tg = NULL;
            }
        }
    };

    static int start(int thread_num_)
    {
        singleton_t<global_data_t>::instance().start(thread_num_);
        return 0;
    }
    static acceptor_i* listen(const string& host_, msg_handler_i* msg_handler_)
    {
        singleton_t<global_data_t>::instance().start();
        acceptor_impl_t* ret = new acceptor_impl_t(&(singleton_t<global_data_t>::instance().epoll),
                                                   msg_handler_, 
                                                   (singleton_t<global_data_t>::instance().tg));
        
        if (ret->open(host_))
        {
            delete ret;
            return NULL;
        }
        singleton_t<global_data_t>::instance().all_acceptor.push_back(ret);
        return ret;
    }
    static acceptor_i* gateway_listen(const string& host_, msg_handler_i* msg_handler_)
    {
        singleton_t<global_data_t>::instance().start();
        acceptor_impl_t* ret = new gateway_acceptor_t(&(singleton_t<global_data_t>::instance().epoll),
                                                   msg_handler_, 
                                                   (singleton_t<global_data_t>::instance().tg));
        
        if (ret->open(host_))
        {
            delete ret;
            return NULL;
        }
        singleton_t<global_data_t>::instance().all_acceptor.push_back(ret);
        return ret;
    }
    static acceptor_i* http_listen(const string& host_, msg_handler_i* msg_handler_)
    {
        singleton_t<global_data_t>::instance().start();
        acceptor_impl_t* ret = new http_acceptor_t(&(singleton_t<global_data_t>::instance().epoll),
                                                   msg_handler_, 
                                                   (singleton_t<global_data_t>::instance().tg));
        
        if (ret->open(host_))
        {
            delete ret;
            return NULL;
        }
        singleton_t<global_data_t>::instance().all_acceptor.push_back(ret);
        return ret;
    }
    static socket_ptr_t connect(const string& host_, msg_handler_i* msg_handler_)
    {
        singleton_t<global_data_t>::instance().start();
        return connector_t::connect(host_, &(singleton_t<global_data_t>::instance().epoll), msg_handler_,
                                    (singleton_t<global_data_t>::instance().tg->rand_alloc()));
    }
};

}

#endif
