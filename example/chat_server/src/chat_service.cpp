
#include <stdarg.h>
#include <stdio.h>
#include <iostream>
using namespace std;

#include "chat_service.h"

static int log_impl(const char* mod, const char* fmt, ...)
{
    char buff[256];
    int len = snprintf(buff, sizeof(buff), "%s ", mod);

    va_list vl;
    va_start(vl, fmt);
    vsnprintf(buff + len, sizeof(buff) - len - 1, fmt, vl);
    va_end(vl);
    printf(buff);
    return printf("\n");
}

#define logtrace(content) log_impl content
#define CHAT_SERVICE "CHAT_SERVICE"

chat_service_t::chat_service_t():
    m_msg_dispather(*this)
{
}

chat_service_t::~chat_service_t()
{
}

int chat_service_t::handle_broken(socket_ptr_t sock_)
{
    uid_t* user = sock_->get_data<uid_t>();
    if (NULL == user)
    {
        sock_->safe_delete();
        return 0;
    }

    lock_guard_t lock(m_mutex);
    m_clients.erase(*user);

    user_logout_ret_t ret_msg;
    ret_msg.uid = *user;
    string json_msg = ret_msg.encode_json();
    sock_->safe_delete();

    map<uid_t, socket_ptr_t>::iterator it = m_clients.begin();
    for (; it != m_clients.end(); ++it)
    {
        it->second->async_send(json_msg);
    }
    return 0;
}


int chat_service_t::handle_msg(const message_t& msg_, socket_ptr_t sock_)
{
    try
    {
        m_msg_dispather.dispath(msg_.get_body() , sock_);
    }
    catch(exception& e)
    {
        sock_->async_send("msg not supported!");
        logtrace((CHAT_SERVICE, "chat_service_t::handle_msg exception<%s>", e.what()));
        sock_->close();
    }
    return 0;
}

int chat_service_t::handle(shared_ptr_t<login_req_t> req_, socket_ptr_t sock_)
{
    logtrace((CHAT_SERVICE, "chat_service_t::handle login_req_t uid<%u>", req_->uid));
    lock_guard_t lock(m_mutex);

    pair<map<uid_t, socket_ptr_t>::iterator, bool> ret =  m_clients.insert(make_pair(req_->uid, sock_));
    if (false == ret.second)
    {
        logtrace((CHAT_SERVICE, "chat_service_t::handle login_req_t insert failed"));
        sock_->close();
        return -1;
    }

    uid_t* user = new uid_t(req_->uid);
    sock_->set_data(user);

    user_login_ret_t login_ret;
    login_ret.uid = req_->uid;
    string login_json = login_ret.encode_json();

    online_list_ret_t online_list;

    map<uid_t, socket_ptr_t>::iterator it = m_clients.begin();
    for (; it != m_clients.end(); ++it)
    {
        online_list.uids.push_back(it->first);
        it->second->async_send(login_json);
    }

    sock_->async_send(online_list.encode_json());
    return 0;
}

int chat_service_t::handle(shared_ptr_t<chat_to_some_req_t> req_, socket_ptr_t sock_)
{
    logtrace((CHAT_SERVICE, "chat_service_t::handle chat_to_some"));
    uid_t* user = sock_->get_data<uid_t>();
    if (NULL == user) return -1;

    lock_guard_t lock(m_mutex);

    chat_content_ret_t content_ret;
    content_ret.from_uid = *user;
    content_ret.content  = req_->content;

    string json_msg =  content_ret.encode_json();
    for (size_t i = 0; i < req_->dest_uids.size(); ++i)
    {
        map<uid_t, socket_ptr_t>::const_iterator it = m_clients.find(req_->dest_uids[i]);
        if (it != m_clients.end())
        {
            it->second->async_send(json_msg);
        }
    }
    return 0;
}
