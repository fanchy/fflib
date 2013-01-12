#ifndef _SOCKET_OP_H_
#define _SOCKET_OP_H_

#include <fcntl.h>
#include <errno.h>

namespace ff {

struct socket_op_t
{
    static int set_nonblock(int fd_)
    {
        int flags;
        flags = fcntl(fd_, F_GETFL, 0);
        if ((flags = fcntl(fd_, F_SETFL, flags | O_NONBLOCK)) < 0)
        {
            return -1;
        }
    
        return 0;
    }
};

}

#endif
