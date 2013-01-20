#ifndef _COMMON_SOCKET_CONTROLLER_H_
#define _COMMON_SOCKET_CONTROLLER_H_

#include <string>
using namespace std;

#include "net/socket_controller_i.h"
#include "net/message.h"
#include "net/msg_handler_i.h"

namespace ff {

class socket_i;

class common_socket_controller_t: public socket_controller_i
{
public:
    common_socket_controller_t(msg_handler_ptr_t msg_handler_);
    ~common_socket_controller_t();
    virtual int handle_error(socket_i*);
    virtual int handle_read(socket_i*, char* buff, size_t len);
    virtual int handle_write_completed(socket_i*);

    virtual int check_pre_send(socket_i*, string& buff_);

protected:
    msg_handler_ptr_t   m_msg_handler;
    size_t              m_have_recv_size;
    message_t           m_message;
};

}

#endif
