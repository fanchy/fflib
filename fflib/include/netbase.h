#ifndef _NET_BASE_H_
#define _NET_BASE_H_

namespace ff {

//! 文件描述符相关接口
typedef int socket_fd_t;
class fd_i
{
public:
    virtual ~fd_i(){}

    virtual socket_fd_t socket() 	 = 0;
    virtual int handle_epoll_read()  = 0;
    virtual int handle_epoll_write() = 0;
    virtual int handle_epoll_del() 	 = 0;

    virtual void close() 			 = 0;
};

//! 事件分发器
class io_demultiplexer_i
{
public:
    virtual ~io_demultiplexer_i(){}

    virtual int event_loop() 		    = 0;
    virtual int close() 				= 0;
    virtual int register_fd(fd_i*)      = 0;
    virtual int unregister_fd(fd_i*)  	= 0;
    virtual int mod_fd(fd_i*)           = 0;
};

//typedef fd_i epoll_fd_i;
}

#endif


