#ifndef _GATEWAY_SOCKET_CONTROLLER_H_
#define _GATEWAY_SOCKET_CONTROLLER_H_

#include "net/common_socket_controller.h"

namespace ff {

class net_stat_t;

    class gateway_socket_controller_t: public common_socket_controller_t
{
public:
    gateway_socket_controller_t(msg_handler_ptr_t msg_handler_, net_stat_t*);
    ~gateway_socket_controller_t();

    virtual int handle_open(socket_i*);
    virtual int handle_read(socket_i*, char* buff, size_t len);
    virtual int handle_error(socket_i*);

private:
    net_stat_t*     m_net_stat;
    time_t          m_last_update_tm;
};

}
#endif
