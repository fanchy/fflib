
//!  连接器
#ifndef _CONNECTOR_H_
#define _CONNECTOR_H_

#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#include "net/socket_impl.h"
#include "base/strtool.h"
#include "net/msg_handler_i.h"
#include "net/common_socket_controller.h"

namespace ff {

class connector_t
{
public:
    static socket_ptr_t connect(const string& host_, epoll_i* e_, msg_handler_i* msg_handler_, task_queue_i* tq_)
    {
        socket_ptr_t ret = NULL;
        //! example tcp://192.168.1.1:1024
        vector<string> vt;
        strtool::split(host_, vt, "://");
        assert(vt.size() == 2);
        
        vector<string> vt2;
        strtool::split(vt[1], vt2, ":");
        assert(vt2.size() == 2);
        
        int s;
        struct sockaddr_in addr;
        
        if((s = socket(AF_INET,SOCK_STREAM,0)) < 0)
        {
            perror("socket");
            return ret;
        }
        /* 填写sockaddr_in结构*/
        bzero(&addr,sizeof(addr));
        addr.sin_family      = AF_INET;
        addr.sin_port        = htons(atoi(vt2[1].c_str()));
        addr.sin_addr.s_addr = inet_addr(vt2[0].c_str());
        /* 尝试连线*/
        if(::connect(s, (struct sockaddr *)&addr, sizeof(addr)) < 0)
        {
            perror("connect");
            return ret;
        }
        
        ret = new socket_impl_t(e_, new common_socket_controller_t(msg_handler_), s, tq_);
        ret->open();
        return ret;
    }

};
    
}
#endif
