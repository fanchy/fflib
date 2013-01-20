
#ifndef _RPC_FUTURE_H_
#define _RPC_FUTURE_H_

namespace ff {

#include "rpc/rpc_callback.h"
#include "rpc/rpc_service.h"
#include "base/lock.h"

template<typename RET>
class rpc_future_t
{
public:
    rpc_future_t():m_cond(m_mutex),m_flag(false)
    {}
    void callback(RET& out_)
    {
        m_msg = out_;
        lock_guard_t lock(m_mutex);
        m_flag = true;
        m_cond.signal();
    }
    
    template<typename MSGT>
    RET call(rpc_service_t* rs_, MSGT& in_)
    {
        rs_->async_call(in_, singleton_t<msg_traits_t<MSGT> >::instance().get_id(),
                        binder_t::callback(&rpc_future_t<RET>::callback, this));
        lock_guard_t lock(m_mutex);
        while(false == m_flag)
        {
            m_cond.wait();
        }
        m_flag = false;
        return m_msg;
    }

private:
    RET             m_msg;
    condition_var_t m_cond;
    mutex_t         m_mutex;
    bool            m_flag;
};

}

#endif
