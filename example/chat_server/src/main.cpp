
#include<stdio.h>
#include<pthread.h>
#include <iostream>
using namespace std;

#include "detail/acceptor_impl.h"
#include "detail/epoll_impl.h"
#include "thread.h"
#include "smart_ptr/shared_ptr.h"
#include "chat_service.h"
#include "net_factory.h"

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        cout <<argv[0] <<" useage listen:host port\n";
        return 1;
    }
    char buff[128];
    snprintf(buff, sizeof(buff), "tcp://%s:%s", argv[1], argv[2]);

    task_queue_pool_t tg;
    thread_t thread;
    thread.create_thread(task_queue_pool_t::gen_task(&tg), 2);

    chat_service_t chat_service;

    epoll_impl_t epoll;
//    acceptor_impl_t acceptor(&epoll, &chat_service);
//    ret = acceptor.open(string(buff));

    if (NULL == net_factory_t::listen(buff, &chat_service))
    {
        cout <<"acceptor open failed:" << buff <<"\n";
        return 1;
    }
    else
    {
        cout <<"acceptor open ok, wait to listen\n";
    }

    epoll.event_loop();
    thread.join();

    return 0;
}

