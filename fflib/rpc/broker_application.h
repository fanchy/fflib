#ifndef _BROKER_APPLICATION_H_
#define _BROKER_APPLICATION_H_

#include "rpc/broker_service.h"
#include "net/net_factory.h"
#include "base/arg_helper.h"

namespace ff {

class broker_application_t
{
public:
    struct app_info_t
    {
        app_info_t():acceptor(NULL)
        {}
        broker_service_t broker_service;
        acceptor_i*      acceptor;
    };
    static int run(int argc, char** argv)
    {
        arg_helper_t arg(argc, argv);

        if (arg.get_option_value("-l").empty())
        {
            cerr <<"usage: app -l tcp://127.0.0.1:10241\n";
            exit(-1);
        }

        net_factory_t::start(1);

        singleton_t<app_info_t>::instance().acceptor = net_factory_t::listen(arg.get_option_value("-l"), &(singleton_t<app_info_t>::instance().broker_service));

        assert(singleton_t<app_info_t>::instance().acceptor && "can not listen this address");

        if (arg.get_option_value("-node") == "slave")
        {
            singleton_t<app_info_t>::instance().broker_service.init_slave(arg.get_option_value("-master_host"), arg.get_option_value("-l"));
        }
        return 0;
    }
};

}
#endif
