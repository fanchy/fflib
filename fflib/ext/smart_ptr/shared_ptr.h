#ifndef _SHARED_PTR_H_
#define _SHARED_PTR_H_

#include "atomic_op.h"
#include <assert.h>

namespace ff {

template<typename T>
class shared_ptr_t
{
public:
    typedef T                         object_t;
    typedef shared_ptr_t<T>           self_type_t;
public:
    shared_ptr_t();
    shared_ptr_t(object_t* p);
    shared_ptr_t(self_type_t& p);
    template<typename R>
    shared_ptr_t(shared_ptr_t<R>& p):
        m_dest_ptr(p.get()),
        m_ref_count(p.ger_ref_count())
    {
        if (NULL != m_dest_ptr)
        {
            m_ref_count->inc();
        }
    }

    ~shared_ptr_t();

    ref_count_t* ger_ref_count()  const  { return m_ref_count; }
    size_t       ref_count() const       { return m_ref_count != NULL? (size_t)m_ref_count->value(): 0; }
    object_t*    get() const             { return m_dest_ptr; }
    void         reset();
    object_t&    operator*();
    object_t*    operator->();
    bool         operator==(const self_type_t& p);
    bool         operator==(const object_t* p);
    self_type_t& operator=(const self_type_t& src_);
    
    operator bool() const
    {
        return NULL != m_dest_ptr;
    }

private:
    object_t*         m_dest_ptr;
    ref_count_t*      m_ref_count;
};

template<typename T>
shared_ptr_t<T>::shared_ptr_t():
    m_dest_ptr(NULL),
    m_ref_count(NULL)
{
}

template<typename T>
shared_ptr_t<T>::shared_ptr_t(object_t* p):
    m_dest_ptr(p),
    m_ref_count(NULL)
{
    if (NULL != m_dest_ptr)
    {
        m_ref_count = new ref_count_t();
        m_ref_count->inc();
    }
}

template<typename T>
shared_ptr_t<T>::shared_ptr_t(self_type_t& p):
    m_dest_ptr(p.get()),
    m_ref_count(p.ger_ref_count())
{
    if (NULL != m_dest_ptr)
    {
        m_ref_count->inc();
    }
}

template<typename T>
shared_ptr_t<T>::~shared_ptr_t()
{
    reset();
}

template<typename T>
void shared_ptr_t<T>::reset()
{
    if (m_dest_ptr)
    {
        if (true == m_ref_count->dec_and_check_zero())
        {
            delete m_ref_count;
            delete m_dest_ptr;
        }
        m_ref_count = NULL;
        m_dest_ptr = NULL;
    }
}

template<typename T>
typename shared_ptr_t<T>::object_t&    shared_ptr_t<T>::operator*()
{
    assert(NULL != m_dest_ptr);
    return *m_dest_ptr;
}

template<typename T>
typename shared_ptr_t<T>::object_t*    shared_ptr_t<T>::operator->()
{
    assert(NULL != m_dest_ptr);
    return m_dest_ptr;
}

template<typename T>
bool shared_ptr_t<T>::operator==(const self_type_t& p)
{
    return m_dest_ptr == p.get();
}

template<typename T>
bool shared_ptr_t<T>::operator==(const object_t* p)
{
    return m_dest_ptr == p;
}

template<typename T>
typename shared_ptr_t<T>::self_type_t& shared_ptr_t<T>::operator=(const self_type_t& src_)
{
    reset();
    m_dest_ptr = src_.get();
    m_ref_count = src_.ger_ref_count();
    if (NULL != m_dest_ptr)
    {
        m_ref_count->inc();
    }
    return *this;
}
    
}

#endif
