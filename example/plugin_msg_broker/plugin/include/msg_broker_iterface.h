#ifndef _MSG_BROKER_INTERFACE_H_
#define _MSG_BROKER_INTERFACE_H_

#include <string>
using namespace std;

class message_t
{
public:
    const string& get_body() const;
};

class channel_t
{
public:
    void  set_data(void* p);
    void* get_data() const;

    template<typename T>
    T* get_data() const { return (T*)this->get_data(); }

    void async_send(const string& buff_);
    void close();
};
typedef channel_t* channel_ptr_t;
#endif
