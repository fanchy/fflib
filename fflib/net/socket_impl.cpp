#include <errno.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "net/socket_impl.h"
#include "net/epoll_i.h"
#include "net/socket_controller_i.h"
#include "net/socket_op.h"
#include "base/singleton.h"
#include "base/lock.h"
#include "base/task_queue_i.h"

using namespace ff;

socket_impl_t::socket_impl_t(epoll_i* e_, socket_controller_i* seh_, int fd_, task_queue_i* tq_):
    m_epoll(e_),
    m_sc(seh_),
    m_fd(fd_),
    m_tq(tq_)
{
}

socket_impl_t::~socket_impl_t()
{
    delete m_sc;
}

void socket_impl_t::open()
{
    socket_op_t::set_nonblock(m_fd);
    m_sc->handle_open(this);
    async_recv();
}

void socket_impl_t::close()
{
    m_tq->produce(task_binder_t::gen(&socket_impl_t::close_impl, this));
}

void socket_impl_t::close_impl()
{
    if (m_fd > 0)
    {
        m_epoll->unregister_fd(this);
        ::close(m_fd);
        m_fd = -1;
    }
}

int socket_impl_t::handle_epoll_read()
{
    m_tq->produce(task_binder_t::gen(&socket_impl_t::handle_epoll_read_impl, this));
    return 0;
}

int socket_impl_t::handle_epoll_read_impl()
{
    if (is_open())
    {
        int nread = 0;
        char recv_buffer[RECV_BUFFER_SIZE];
        do
        {
            nread = ::read(m_fd, recv_buffer, sizeof(recv_buffer) - 1);
            if (nread > 0)
            {
                recv_buffer[nread] = '\0';
                m_sc->handle_read(this, recv_buffer, size_t(nread));
                if (nread < int(sizeof(recv_buffer) - 1))
                {
                	break;//! equal EWOULDBLOCK
                }
            }
            else if (0 == nread) //! eof
            {
                this->close();
                return -1;
            }
            else
            {
                if (errno == EINTR)
                {
                    continue;
                }
                else if (errno == EWOULDBLOCK)
                {
                    break;
                }
                else
                {
                    this->close();
                    return -1;
                }
            }
        } while(1);
    }
    return 0;
}

int socket_impl_t::handle_epoll_del()
{
    m_tq->produce(task_binder_t::gen(&socket_controller_i::handle_error, this->get_sc(), this));
    return 0;
}

int socket_impl_t::handle_epoll_write()
{
    m_tq->produce(task_binder_t::gen(&socket_impl_t::handle_epoll_write_impl, this));
    return 0;
}

int socket_impl_t::handle_epoll_write_impl()
{
    int ret = 0;
    string left_buff;

    if (false == is_open() || true == m_send_buffer.empty())
    {
        return 0;
    }

    do
    {
        const string& msg = m_send_buffer.front();
        ret = do_send(msg, left_buff);

        if (ret < 0)
        {
            this ->close();
            return -1;
        }
        else if (ret > 0)
        {
            m_send_buffer.pop_front();
            m_send_buffer.push_front(left_buff);
            return 0;
        }
        else
        {
            m_send_buffer.pop_front();
        }
    } while (false == m_send_buffer.empty());

    m_sc->handle_write_completed(this);
    return 0;
}

void socket_impl_t::async_send(const string& msg_)
{
    m_tq->produce(task_binder_t::gen(&socket_impl_t::send_impl, this, msg_));
}

void socket_impl_t::send_impl(const string& src_buff_)
{
    string buff_ = src_buff_;

    if (false == is_open() || m_sc->check_pre_send(this, buff_))
    {
        return;
    }
    //! socket buff is full, cache the data
    if (false == m_send_buffer.empty())
    {
        m_send_buffer.push_back(buff_);
        return;
    }

    string left_buff;
    int ret = do_send(buff_, left_buff);

    if (ret < 0)
    {
        this ->close();
    }
    else if (ret > 0)
    {
        m_send_buffer.push_back(left_buff);
    }
    else
    {
        //! send ok
        m_sc->handle_write_completed(this);
    }
}

int socket_impl_t::do_send(const string& buff_, string& left_buff_)
{
    size_t nleft             = buff_.size();
    const char* buffer       = buff_.data();
    ssize_t nwritten;

    while(nleft > 0)
    {
        if((nwritten = ::send(m_fd, buffer, nleft, MSG_NOSIGNAL)) <= 0)
        {
            if (EINTR == errno)
            {
                nwritten = 0;
            }
            else if (EWOULDBLOCK == errno)
            {
                left_buff_.assign(buff_.c_str() + (buff_.size() - nleft), nleft);
                return 1;
            }
            else
            {
                this->close();
                return -1;
            }
        }

        nleft    -= nwritten;
        buffer   += nwritten;
    }

    return 0;
}

void socket_impl_t::async_recv()
{
    m_epoll->register_fd(this);
}

void socket_impl_t::safe_delete()
{
    struct lambda_t
    {
        static void exe(void* p_)
        {
            delete ((socket_impl_t*)p_);
        }
    };
    m_tq->produce(task_t(&lambda_t::exe, this));
}
