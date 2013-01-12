#ifndef _PLUGIN_PYTHON_H_
#define _PLUGIN_PYTHON_H_
#include <Python.h>

#include <string>
#include <map>
using namespace std;

#include "plugin_i.h"

using namespace ff;

class plugin_python_t : public plugin_i
{
public:
    plugin_python_t(const string& name_);
    ~plugin_python_t();
    int start();
    int stop();

    int handle_broken(channel_ptr_t channel_);
    int handle_msg(const message_t& msg_, channel_ptr_t channel_);

    channel_ptr_t get_channel(long p);
private:
    int load_py_mod();
    int call_py_handle_msg(long val, const char* msg);
    int call_py_handle_broken(long val);
private:
    string                                          m_py_name;
    PyObject *                                      m_py_mod;
    map<long, channel_ptr_t>                        m_channel_mgr;
};
#endif
