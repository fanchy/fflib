#include<unistd.h>
#include<stdlib.h>
#include<signal.h>
#include<stdio.h>
#include<pthread.h>
#include <iostream>
using namespace std;

#include "net/acceptor_impl.h"
#include "net/epoll_impl.h"
#include "base/thread.h"
#include "base/shared_ptr.h"
#include "msg_broker_service.h"
#include "net/net_factory.h"

void handler(int sig) {
    printf("signal:%d\n", sig);
    exit(0);
}

int main(int argc, char* argv[])
{
    signal(SIGINT,handler);
    if (argc != 4)
    {
        cout <<argv[0] <<" useage listen:host port plugin_path\n";
        return 1;
    }
    char buff[128];
    snprintf(buff, sizeof(buff), "tcp://%s:%s", argv[1], argv[2]);

    task_queue_pool_t tg(1);
    thread_t thread;
    thread.create_thread(task_queue_pool_t::gen_task(&tg), 1);

    msg_broker_service_t broker_service;

    if (broker_service.start(argv[3]))
    {
        return -1;
    }

    epoll_impl_t epoll;
    /*
    acceptor_impl_t acceptor(&epoll, &broker_service);
    ret = acceptor.open(string(buff));
     */
    if (NULL == net_factory_t::listen(buff, &broker_service))
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

