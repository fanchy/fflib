#ifndef _NET_STAT_H_
#define _NET_STAT_H_

#include "net/socket_i.h"
#include "base/arg_helper.h"
#include "base/timer_service.h"
#include "base_heartbeat.h"

#include <stdlib.h>
#include <string>
using namespace std;

namespace ff {

class net_stat_t
{
public:
    net_stat_t();
    ~net_stat_t();

    int start(arg_helper_t& arg_helper_);
    int stop();

    //! 最大消息包大小
    int get_max_packet_size() const { return m_max_packet_size; }

    //! 获取心跳模块引用
    base_heartbeat_t<socket_ptr_t>& get_heartbeat() { return m_heartbeat;}

    //! 处理timer 回调
    void handle_timer_check();
private:
    timer_service_t*                m_timer_service;
    int                             m_max_packet_size;
    base_heartbeat_t<socket_ptr_t>  m_heartbeat;
};

}
#endif
