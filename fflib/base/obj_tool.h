#ifndef _FF_OBJ_TOOL_H_
#define _FF_OBJ_TOOL_H_

#include "base/singleton.h"
#include "base/type_i.h"

#include <list>
#include <map>
#include <fstream>
using namespace std;

namespace ff
{


template<typename T>
T* new_obj()
{
    T* p = new T();
    singleton_t<obj_counter_t<T> >::instance().inc(1);
    return p;
}

template<typename T, typename ARG1>
T* new_obj(ARG1 arg1)
{
    T* p = new T(arg1);
    singleton_t<obj_counter_t<T> >::instance().inc(1);
    return p;
}

template<typename T, typename ARG1, typename ARG2>
T* new_obj(ARG1 arg1, ARG2 arg2)
{
    T* p = new T(arg1, arg2);
    singleton_t<obj_counter_t<T> >::instance().inc(1);
    return p;
}

template<typename T>
T* new_array(int n)
{
    T* p = new T[n];
    singleton_t<obj_counter_t<T> >::instance().inc(n);
    return p;
}

template<typename T>
void del_obj(T* p)
{
    if (p)
    {
        delete p;
        singleton_t<obj_counter_t<T> >::instance().dec(1);
    }
}

template<typename T>
void del_array(T* p, int n)
{
    if (p)
    {
        delete [] p;
        singleton_t<obj_counter_t<T> >::instance().dec(n);
    }
}


}

#endif
