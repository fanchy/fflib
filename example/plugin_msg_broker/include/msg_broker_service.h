#ifndef _MSG_BROKER_SERVICE_H_
#define _MSG_BROKER_SERVICE_H_

#include <map>
using namespace std;

#include "socket_i.h"
#include "msg_handler_i.h"
#include "plugin_i.h"

using namespace ff;

class msg_broker_service_t: public msg_handler_i
{
public:
    msg_broker_service_t();
    ~msg_broker_service_t();

    int handle_broken(socket_ptr_t sock_);
    int handle_msg(const message_t& msg_, socket_ptr_t sock_);

    int start(const string& path_);
    int stop();

private:
    plugin_ptr_t        m_plugin;
};
#endif
