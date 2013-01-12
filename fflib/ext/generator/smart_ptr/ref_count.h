#ifndef _REF_COUNT_H_
#define _REF_COUNT_H_

#define ATOMIC_ADD(src_ptr, v)             (void)__sync_add_and_fetch(src, v)
#define ATOMIC_SUB_AND_FETCH(src_ptr, v) 	__sync_sub_and_fetch(src, v)

class ref_count_t
{
    typedef volatile long atomic_t;
public:
    ref_count_t():
        m_ref_num(1)
    {}
    ~ref_count_t()
    {}

    inline void inc()
    {
        //ATOMIC_ADD(&m_ref_num, 1);
        ++m_ref_num;
    }
    inline bool dec_and_check_zero()
    {
        return 0 == --m_ref_num;//ATOMIC_SUB_AND_FETCH((&m_ref_num, 1);
    }
    inline atomic_t size()
    {
        return m_ref_num;
    }

private:
    atomic_t m_ref_num;
};
#endif
