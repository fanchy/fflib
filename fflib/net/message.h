#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#include <stdint.h>
#include <string>
#include <string.h>
using namespace std;

namespace ff {

struct message_head_t
{
    message_head_t():
        size(0),
        cmd(0),
        flag(0)
    {}
    explicit message_head_t(uint16_t cmd_):
        size(0),
        cmd(cmd_),
        flag(0)
    {}
    uint32_t size;
    uint16_t cmd;
    uint16_t flag;
};

class message_t
{
public:
    message_t()
    {
    }

    uint16_t get_cmd() const               { return m_head.cmd; }
    const string& get_body() const         { return m_body;}
    size_t size() const                    { return m_head.size; }
    uint16_t get_flag() const              { return m_head.flag; }

    size_t append_head(size_t index_, char* buff, size_t len)
    {
        size_t will_append = sizeof(m_head) - index_;
        if (will_append > len) will_append = len;
        ::memcpy((char*)&m_head + index_, buff, will_append);
        return will_append;
    }
    size_t append_msg(char* buff, size_t len)
    {
        size_t will_append = m_head.size - m_body.size();
        if (will_append > len) will_append = len;
        m_body.append(buff, will_append);
        return will_append;
    }
    void clear()
    {
        ::memset(&m_head, 0, sizeof(m_head));
        m_body.clear();
    }
    void append_to_body(const char* buff, size_t len)
    {
        m_body.append(buff, len);
    }
    //! for parse
    bool have_recv_head(size_t have_recv_size_) { return have_recv_size_ >= sizeof(message_head_t);}
private:
    message_head_t m_head;
    string         m_body;
};
    
}
#endif
