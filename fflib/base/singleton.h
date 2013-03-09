#ifndef _SINGLETON_H_
#define _SINGLETON_H_

#include <stdlib.h>
#include <pthread.h>

template<class T>
class singleton_t
{
public:
    static T& instance()
    {
	pthread_once(&m_ponce, &singleton_t::init);
        return *m_instance;
    }

    static T* instance_ptr()
    {
        return m_instance;
    }
private:
    static void init()
    {
        m_instance = new T();
        atexit(destroy);
    }
    static void destroy()
    {
        if(m_instance)
        {
                delete m_instance;
                m_instance = 0;
        }
    }

private:
    static T* volatile m_instance;
    static pthread_once_t m_ponce;
};

template<class T>
T* volatile singleton_t<T>::m_instance = NULL;

template<typename T>
pthread_once_t singleton_t<T>::m_ponce = PTHREAD_ONCE_INIT;

#endif

