#ifndef _TEXT_SOCKET_CONTROLLER_IMPL_H_
#define _TEXT_SOCKET_CONTROLLER_IMPL_H_

#include <string>
using namespace std;

#include "net/socket_controller_i.h"
#include "net/message.h"
#include "net/msg_handler_i.h"

namespace ff {

class socket_i;

class text_socket_controller_impl_t: public socket_controller_i
{
    enum protocol_e
    {
        TXT_PROTOCOL = 0,
        HTTP_PROTOCOL,
        UNKNOWN_PROTOCOL,
    };
public:
    text_socket_controller_impl_t(msg_handler_ptr_t msg_handler_);
    ~text_socket_controller_impl_t();
    virtual int handle_error(socket_i*);
    virtual int handle_read(socket_i*, char* buff, size_t len);
    virtual int handle_write_completed(socket_i*);

    virtual int check_pre_send(socket_i*, string& buff_);
private:
    int parse_msg_head();
    int append_msg_body(socket_i* sp_, char* buff_begin_, size_t& left_);

    //! check which protocol does peer use
    protocol_e analyze_protocol(char* buff, size_t len);
    //! parse http protocol request
    int parse_http_protocol(socket_i* sp_, char* buff, size_t len);
    //! parse simple text protocol request
    int parse_text_protocol(socket_i* sp_, char* buff, size_t len);
private:
    protocol_e          m_protocol;
    msg_handler_ptr_t   m_msg_handler;
    bool                m_head_end_flag;
    size_t              m_body_size;
    string              m_head;
    message_t           m_message;
};
    
}
#endif
