#ifndef _FF_ATOMIC_H_
#define _FF_ATOMIC_H_

namespace ff {
//! windows.h
//! InterlockedIncrement
//! InterlockedDecrement

#define ATOMIC_ADD(src_ptr, v)            (void)__sync_add_and_fetch(src_ptr, v)
#define ATOMIC_SUB_AND_FETCH(src_ptr, v)  __sync_sub_and_fetch(src_ptr, v)
#define ATOMIC_ADD_AND_FETCH(src_ptr, v)  __sync_add_and_fetch(src_ptr, v)
#define ATOMIC_FETCH(src_ptr)             __sync_add_and_fetch(src_ptr, 0)
#define ATOMIC_SET(src_ptr, v)            (void)__sync_bool_compare_and_swap(src_ptr, *(src_ptr), v)

class ref_count_t
{
    typedef  volatile long atomic_t;
public:
    ref_count_t():
        m_ref_num(0)
    {}
    ~ref_count_t()
    {}

    inline void inc(int n = 1)
    {
        ATOMIC_ADD(&m_ref_num, n);
    }
    inline bool dec_and_check_zero(int n = 1)
    {
        return 0 == ATOMIC_SUB_AND_FETCH(&m_ref_num, n);
    }
    inline atomic_t inc_and_fetch(int n = 1)
    {
        return ATOMIC_ADD_AND_FETCH(&m_ref_num, n);
    }
    inline atomic_t value()
    {
        return ATOMIC_FETCH(&m_ref_num);
    }

private:
    atomic_t m_ref_num;
};
    
}

#endif
