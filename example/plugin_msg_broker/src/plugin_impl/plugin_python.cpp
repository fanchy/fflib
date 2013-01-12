
#include "plugin_impl/plugin_python.h"
#include "plugin_impl/pyext.h"
#include "log_module.h"


plugin_python_t::plugin_python_t(const string& name_):
    m_py_mod(NULL)
{
    string pythonpath = "./";
    int pos = name_.find_last_of('/');
    if (-1 == pos)
    {
        m_py_name = name_;
    }
    else
    {
        m_py_name = name_.substr(pos+1);
        pythonpath = name_.substr(0, pos+1);
    }
    pos = m_py_name.find_first_of('.');
    m_py_name = m_py_name.substr(0, pos);

    Py_InitializeEx(0);
    Py_SetPythonHome((char*)pythonpath.c_str());
    initpyext(this);
    PyRun_SimpleString("import channel;import sys;sys.path.append('./plugin/plugin_echo_py/')");
}

plugin_python_t::~plugin_python_t()
{
    Py_Finalize();
}

int plugin_python_t::start()
{
    if(load_py_mod())
    {
        return -1;
    } 
    return 0;
}

int plugin_python_t::stop()
{
    return 0;
}

int plugin_python_t::load_py_mod()
{
    PyObject *pName, *pModule;
    pName = PyString_FromString(m_py_name.c_str());
    pModule = PyImport_Import(pName);
    if (!pModule )  
    {
        Py_DECREF(pName);
        logerror((PLUGIN_IMPL, "can't find %s.py\n", m_py_name.c_str()));
        if (PyErr_Occurred())
    {
        PyErr_Print();
        PyErr_Clear();
        return -1;
    }  
        return -1;
    }
    m_py_mod = PyModule_GetDict(pModule);
    Py_DECREF(pName);
    Py_DECREF(pModule);
    return 0;
}

int plugin_python_t::handle_broken(channel_ptr_t channel_)
{    
    m_channel_mgr.erase(long(channel_));
    delete channel_;
    return call_py_handle_broken(long(channel_));
}
int plugin_python_t::handle_msg(const message_t& msg_, channel_ptr_t channel_)
{
    m_channel_mgr.insert(make_pair((long)channel_, channel_));
    return call_py_handle_msg((long)channel_, msg_.get_body().c_str());
}


int plugin_python_t::call_py_handle_msg(long val, const char* msg)
{
    PyObject *pDict       = m_py_mod;
    const char* func_name = "handle_msg";
    PyObject *pFunc, *arglist, *pRetVal;

    pFunc = PyDict_GetItemString(pDict, func_name);
    if (!pFunc || !PyCallable_Check(pFunc))  
    {  
        logerror((PLUGIN_IMPL, "can't find function [%s]\n", func_name));
        return -1;
    }
    arglist = Py_BuildValue("ls", val, msg);
    pRetVal = PyObject_CallObject(pFunc, arglist);
    Py_DECREF(arglist);
    if (pRetVal)
    {
        Py_DECREF(pRetVal);
    }
    if (PyErr_Occurred())
    {
        PyErr_Print();
        PyErr_Clear();
        return -1;
    }
    return 0;
}

int plugin_python_t::call_py_handle_broken(long val)
{
    PyObject *pDict       = m_py_mod;
    const char* func_name = "handle_broken";
    PyObject *pFunc, *arglist, *pRetVal;

    pFunc = PyDict_GetItemString(pDict, func_name);
    if (!pFunc || !PyCallable_Check(pFunc))  
    {  
        logerror((PLUGIN_IMPL, "can't find function [%s]\n", func_name));
        return -1;
    }
    arglist = Py_BuildValue("l", val);
    pRetVal = PyObject_CallObject(pFunc, arglist);
    Py_DECREF(arglist);
    if (pRetVal)
    {
        Py_DECREF(pRetVal);
    }
    if (PyErr_Occurred())
    {
        PyErr_Print();
        PyErr_Clear();
        return -1;
    }
    return 0;
}

channel_ptr_t plugin_python_t::get_channel(long p)
{
    map<long, channel_ptr_t>::iterator it = m_channel_mgr.find(p);
    if (it != m_channel_mgr.end())
    {
        return it->second;
    }
    return NULL;
}
