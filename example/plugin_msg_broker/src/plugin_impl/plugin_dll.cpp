#include <dlfcn.h>
#include <stdlib.h>

#include "log_module.h"
#include "plugin_impl/plugin_dll.h"

plugin_dll_t::plugin_dll_t(const string& name_):
    m_dll_name(name_),
    m_dll_handler(NULL),
    m_msg_cb(NULL),
    m_broken_cb(NULL)
{
}

plugin_dll_t::~plugin_dll_t()
{
}

int plugin_dll_t::start()
{
    m_dll_handler = ::dlopen(m_dll_name.c_str(), RTLD_NOW|RTLD_GLOBAL);

    if (NULL == m_dll_handler)
    {
        logerror((PLUGIN_IMPL, "plugin_dll_t::start dlopen failed:<%s>", dlerror()));
        return -1;
    }

    m_msg_cb     = (handle_channel_msg_func_t)::dlsym(m_dll_handler, HANDLE_CHANNEL_MSG);
    m_broken_cb = (handle_channel_broken_func_t)::dlsym(m_dll_handler, HANDLE_CHANNEL_BROKEN);

    if (NULL == m_msg_cb)
    {
        logerror((PLUGIN_IMPL, "plugin_dll_t::start dlopen failed:<%s> not exist", HANDLE_CHANNEL_MSG));
        return -1;
    }
    if (NULL == m_broken_cb)
    {
        logerror((PLUGIN_IMPL, "plugin_dll_t::start dlopen failed:<%s> not exist", HANDLE_CHANNEL_BROKEN));
        return -1;
    }

    return 0;
}

int plugin_dll_t::stop()
{
    ::dlclose(m_dll_handler);
    return 0;
}

int plugin_dll_t::handle_broken(channel_ptr_t channel_)
{
    return m_broken_cb(channel_);
}

int plugin_dll_t::handle_msg(const message_t& msg_, channel_ptr_t channel_)
{
    return m_msg_cb(msg_, channel_);
}
