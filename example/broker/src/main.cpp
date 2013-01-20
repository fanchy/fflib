#include <iostream>
using namespace std;

#include "rpc/broker_service.h"
#include "net/net_factory.h"

#include "rpc/broker_application.h"
using namespace ff;

int main(int argc, char* argv[])
{
    broker_application_t::run(argc, argv);

    signal_helper_t::wait();
    cout << "\noh end\n";
    return 0;
}
