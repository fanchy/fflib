
#ifndef _PYEXT_H_
#define _PYEXT_H_
#include "plugin_impl/plugin_python.h"

extern "C" {

static plugin_python_t* g_plugin_py_obj = NULL;
static PyObject *
channel_send(PyObject *self, PyObject *args)
{
    long ptr;    
    const char *msg;

    if (!PyArg_ParseTuple(args, "ls", &ptr, &msg))
        return NULL;

    channel_ptr_t c = g_plugin_py_obj->get_channel(ptr);
    if (c)
    {
        c->async_send(msg);
    }
    return Py_BuildValue("l", 0);
}

static PyMethodDef ChannelMethods[] = {
    {"send",  channel_send, METH_VARARGS,
     "send msg."},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

PyMODINIT_FUNC
initpyext(plugin_python_t* obj)
{
    PyObject *m;

    g_plugin_py_obj = obj;
    m = Py_InitModule("channel", ChannelMethods);
    if (m == NULL)
        return;
}

}
#endif
