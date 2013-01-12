#ifndef _CHANNEL_H_
#define _CHANNEL_H_

#include "socket_i.h"

using namespace ff;

class channel_t
{
public:
    channel_t(socket_ptr_t sock_);
    ~channel_t();
    void  set_data(void* p);
    void* get_data() const;

    template<typename T>
    T* get_data() const { return (T*)this->get_data(); }

    void async_send(const string& buff_);
    void close();

private:
    socket_ptr_t    m_socket;
    void*               m_data;
};

typedef channel_t* channel_ptr_t;
#endif
