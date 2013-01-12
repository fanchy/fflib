#include <Python.h>


static PyObject *
spam_test(PyObject *self, PyObject *args)
{
    const char *command;
    int sts;

    if (!PyArg_ParseTuple(args, "s", &command))
        return NULL;
    sts = system(command);
    return Py_BuildValue("i", sts);
}

static PyObject *
channel_send(PyObject *self, PyObject *args)
{
    long ptr;    
    const char *command;
    int sts;

    if (!PyArg_ParseTuple(args, "ls", &ptr, &command))
        return NULL;
    sts = system(command);
    return Py_BuildValue("l", ptr);
}

static PyObject *SpamError;

static PyMethodDef SpamMethods[] = {
    {"system",  spam_test, METH_VARARGS,
     "Execute a shell command."},
    {"send",  channel_send, METH_VARARGS,
     "send msg."},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

PyMODINIT_FUNC
initspam(void)
{
    PyObject *m;

    m = Py_InitModule("channle", SpamMethods);
    if (m == NULL)
        return;
}

PyObject* load_py(const char* file_name)
{
    PyObject *pName, *pModule, *pDict;
    pName = PyString_FromString(file_name);
    pModule = PyImport_Import(pName);
    if (!pModule )  
    {
       Py_DECREF(pName);
	    printf("can't find %s.py\n", file_name);  
	    return NULL;  
    }
    pDict = PyModule_GetDict(pModule);
    Py_DECREF(pName);
    Py_DECREF(pModule);
    return pDict;
}

int call_py_func(PyObject *pDict, const char* func_name, long ptr, const char* msg)
{
    PyObject *pFunc, *arglist, *pRetVal;
    pFunc = PyDict_GetItemString(pDict, func_name);
    if (!pFunc || !PyCallable_Check(pFunc))  
    {  
	    printf("can't find function [%s]\n", func_name);  
	    return -1;
    }
    arglist = Py_BuildValue("ls", ptr, msg);
    pRetVal = PyObject_CallObject(pFunc, arglist);
    Py_DECREF(arglist);
    if (pRetVal)
        Py_DECREF(pRetVal);
    if (PyErr_Occurred())
    {
        PyErr_Print();
        PyErr_Clear();
    }
    return 0;
}

int call_py()
{
    PyObject *pName, *pModule, *pDict, *pFunc, *arglist, *pRetVal;
    pName = PyString_FromString("pytest");
    pModule = PyImport_Import(pName);
    if ( !pModule )  
    {  
	    printf("can't find pytest.py");  
	    return -1;  
    }
    pDict = PyModule_GetDict(pModule);
    pFunc = PyDict_GetItemString(pDict, "test");
    if ( !pFunc || !PyCallable_Check(pFunc) )  
    {  
	    printf("can't find function [add]");  
	    return -1;  
    }
    arglist = Py_BuildValue("()");
    pRetVal = PyObject_CallObject(pFunc, arglist);
    printf("function return value : %ld\r\n", PyInt_AsLong(pRetVal));
    Py_DECREF(pName);
    Py_DECREF(arglist);
    Py_DECREF(pModule); 
    Py_DECREF(pRetVal);
    return 0;
}


int
main(int argc, char *argv[])
{
    Py_SetProgramName(argv[0]);

    Py_Initialize();

    initspam();
    PyRun_SimpleString("import channle;import sys;sys.path.append('./')");

    //PyRun_SimpleString("import spam;print(dir(spam));spam.system(\"ls\")");
    //PyRun_SimpleString("import noddy;print(noddy.Noddy())");

    // call_py();
    PyObject * g_dict = load_py("pytest");
    call_py_func(g_dict, "test", 123, "oh nice");
    Py_Finalize();
}

