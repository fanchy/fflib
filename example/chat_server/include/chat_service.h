#ifndef _CHAT_SERVICE_H_
#define _CHAT_SERVICE_H_

#include <map>
using namespace std;

#include "socket_i.h"
#include "msg_def.h"
#include "msg_handler_i.h"
#include "lock.h"

using namespace ff;

typedef  uint32_t    uid_t;
typedef socket_i*    socket_ptr_t;

class chat_service_t: public msg_handler_i
{
    typedef msg_dispather_t<chat_service_t, socket_ptr_t>* msg_dispather_ptr_t;
public:
    chat_service_t();
    ~chat_service_t();

    int handle_broken(socket_ptr_t sock_);
    int handle_msg(const message_t& msg_, socket_ptr_t sock_);

    int handle(shared_ptr_t<login_req_t> req_, socket_ptr_t sock_);
    int handle(shared_ptr_t<chat_to_some_req_t> req_, socket_ptr_t sock_);

private:
     msg_dispather_t<chat_service_t, socket_ptr_t> m_msg_dispather;
     mutex_t                                                            m_mutex;
    map<uid_t, socket_ptr_t>                                  m_clients;
};
#endif
