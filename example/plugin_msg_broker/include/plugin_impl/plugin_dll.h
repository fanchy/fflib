#ifndef _PLUGIN_IMPL_H_
#define _PLUGIN_IMPL_H_

#include <string>
using namespace std;

#include "plugin_i.h"

using namespace ff;

class plugin_dll_t : public plugin_i
{
public:
    plugin_dll_t(const string& name_);
    ~plugin_dll_t();
    int start();
    int stop();

    int handle_broken(channel_ptr_t channel_);
    int handle_msg(const message_t& msg_, channel_ptr_t channel_);

private:
    string                                          m_dll_name;
    void*                                           m_dll_handler;
    handle_channel_msg_func_t        m_msg_cb;
    handle_channel_broken_func_t    m_broken_cb;
};
#endif
